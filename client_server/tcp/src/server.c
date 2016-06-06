#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event.h>

#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

struct stat {
    int pps;
    int ts;
};

static void on_stats(evutil_socket_t fd, short event, void *arg) {
    struct stat* stat = arg;
    fprintf(stdout, "%d %d\n", stat->ts, stat->pps);
    stat->pps = 0;
    stat->ts++;
}

static void handle_SIGTERM(int sig, short ev, void* arg) {
    printf("Caught SIGTERM %d\n", sig);
    struct event_base* base = arg;
    event_base_loopexit(base, NULL);
}

static void handle_SIGINT(int sig, short ev, void* arg) {
    printf("Caught SIGINT %d\n", sig);
    struct event_base* base = arg;
    event_base_loopexit(base, NULL);
}

static void
echo_read_cb(struct bufferevent *bev, void *ctx)
{
    struct stat *stat = ctx;
    /* This callback is invoked when there is data to read on bev. */
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);

    /* Copy all the data from the input buffer to the output buffer. */
    evbuffer_add_buffer(output, input);
    stat->pps++;
}

static void
echo_event_cb(struct bufferevent *bev, short events, void *ctx)
{
    if (events & BEV_EVENT_ERROR)
        perror("Error from bufferevent");
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        bufferevent_free(bev);
    }
}

static void
accept_conn_cb(struct evconnlistener *listener,
    evutil_socket_t fd, struct sockaddr *address, int socklen,
    void *ctx)
{
    struct stat *stat = ctx;
    /* We got a new connection! Set up a bufferevent for it. */
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(
            base, fd, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, stat);

    bufferevent_enable(bev, EV_READ|EV_WRITE);
    bufferevent_priority_set(bev, 2);
}

static void
accept_error_cb(struct evconnlistener *listener, void *ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got an error %d (%s) on the listener. "
            "Shutting down.\n", err, evutil_socket_error_to_string(err));

    event_base_loopexit(base, NULL);
}

int
main(int argc, char **argv)
{
    struct event_base *base;
    struct evconnlistener *listener;
    struct sockaddr_in sin;
    struct stat stat = {.pps = 0, .ts = 0};

    int port = 9876;

    if (argc > 1) {
        port = atoi(argv[1]);
    }
    if (port<=0 || port>65535) {
        puts("Invalid port");
        return 1;
    }

    base = event_base_new();
    if (!base) {
        puts("Couldn't open event base");
        return 1;
    }

    /* Clear the sockaddr before using it, in case there are extra
     * platform-specific fields that can mess us up. */
    memset(&sin, 0, sizeof(sin));
    /* This is an INET address */
    sin.sin_family = AF_INET;
    /* Listen on 0.0.0.0 */
    sin.sin_addr.s_addr = htonl(0);
    /* Listen on the given port. */
    sin.sin_port = htons(port);

    listener = evconnlistener_new_bind(base, accept_conn_cb, &stat,
        LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
        (struct sockaddr*)&sin, sizeof(sin));
    if (!listener) {
        perror("Couldn't create listener");
        return 1;
    }
    evconnlistener_set_error_cb(listener, accept_error_cb);
    event_base_priority_init(base, 4);

    struct timeval one_second = {1, 0};
    struct event *stats_ev = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, on_stats, &stat);
    event_add(stats_ev, &one_second);
    event_priority_set(stats_ev, 0);

    struct event *sigterm_ev = evsignal_new(base, SIGTERM, handle_SIGTERM, base);
    evsignal_add(sigterm_ev, NULL);
    event_priority_set(sigterm_ev, 1);

    struct event *sigint_ev = evsignal_new(base, SIGINT, handle_SIGINT, base);
    evsignal_add(sigint_ev, NULL);
    event_priority_set(sigint_ev, 1);

    event_base_dispatch(base);

    event_free(stats_ev);
    event_free(sigterm_ev);
    event_free(sigint_ev);
    evconnlistener_free(listener);
    event_base_free(base);

    return 0;
}