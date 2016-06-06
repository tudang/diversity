#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "uthash.h"
#include <signal.h>

struct request_entry {
    int request_id;
    struct bufferevent *bev;
    UT_hash_handle hh;
};

struct proxy_server
{
    int proxy_id;
    int current_request_id;
    int at_second;
    int message_per_second;
    struct request_entry *request_table;
};

static void on_stats(evutil_socket_t fd, short event, void *arg) {
    struct proxy_server *stat = arg;
    fprintf(stdout, "%d %d\n", stat->at_second, stat->message_per_second);
    stat->message_per_second = 0;
    stat->at_second++;
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

void respond_cb(struct proxy_server *proxy, int parm_req_id) {
    int request_id = parm_req_id;
    struct request_entry *s;
    HASH_FIND_INT(proxy->request_table, &request_id, s);
    if (s != NULL) {
        /* This callback is invoked when there is data to read on bev. */
        struct evbuffer *input = bufferevent_get_input(s->bev);
        struct evbuffer *output = bufferevent_get_output(s->bev);
        /* Copy all the data from the input buffer to the output buffer. */
        evbuffer_add_buffer(output, input);
        HASH_DEL(proxy->request_table, s);
        free(s);
    }
}

static void
echo_read_cb(struct bufferevent *bev, void *ctx)
{
    struct proxy_server *proxy = ctx;
    struct request_entry *s = malloc(sizeof(struct request_entry));
    s->request_id = proxy->current_request_id;
    s->bev = bev;
    HASH_ADD_INT(proxy->request_table, request_id, s);

    respond_cb(proxy, proxy->current_request_id);

    proxy->message_per_second++;
    proxy->current_request_id++;
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
    struct proxy_server *proxy = ctx;
    /* We got a new connection! Set up a bufferevent for it. */
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(
            base, fd, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, proxy);

    bufferevent_enable(bev, EV_READ|EV_WRITE);
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


void proxy_free(struct proxy_server *proxy) {
    struct request_entry *s, *tmp;
    HASH_ITER(hh, proxy->request_table, s, tmp) {
        HASH_DEL(proxy->request_table, s);
        free(s);
    }
    free(proxy);
}

int
main(int argc, char **argv)
{
    struct event_base *base;
    struct evconnlistener *listener;
    struct sockaddr_in sin;

    int port = 9876;

    if (argc > 1) {
        port = atoi(argv[1]);
    }
    if (port<=0 || port>65535) {
        puts("Invalid port");
        return 1;
    }

    struct proxy_server *proxy = malloc(sizeof(struct proxy_server));
    proxy->proxy_id = 0;
    proxy->current_request_id = 0;
    proxy->at_second = 0;
    proxy->message_per_second = 0;
    proxy->request_table = NULL;

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

    listener = evconnlistener_new_bind(base, accept_conn_cb, proxy,
        LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
        (struct sockaddr*)&sin, sizeof(sin));
    if (!listener) {
        perror("Couldn't create listener");
        return 1;
    }
    evconnlistener_set_error_cb(listener, accept_error_cb);

    event_base_priority_init(base, 4);

    struct timeval one_second = {1, 0};
    struct event *stats_ev = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, on_stats, proxy);
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

    proxy_free(proxy);

    return 0;
}