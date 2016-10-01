#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define VLEN 32
#define TIMEOUT 1
#define BUFSIZE 128

static struct mmsghdr msgs[VLEN];
static struct iovec iovecs[VLEN];
static char bufs[VLEN][BUFSIZE+1];
static struct sockaddr_in remotes[VLEN];
static struct timespec timeout;

struct server_context {
    struct event_base *base;
    int at_second;
    int message_per_second;
};

static void init_msghdr() {
    int i;
    memset(msgs, 0, sizeof(msgs));
    memset(remotes, 0, sizeof(remotes));
    size_t sa_len = sizeof(struct sockaddr_in);
    for (i = 0; i < VLEN; i++) {
        iovecs[i].iov_base         = bufs[i];
        iovecs[i].iov_len          = BUFSIZE;
        msgs[i].msg_hdr.msg_iov    = &iovecs[i];
        msgs[i].msg_hdr.msg_iovlen = 1;
        msgs[i].msg_hdr.msg_name    = &remotes[i];
        msgs[i].msg_hdr.msg_namelen = sa_len;
    }
    timeout.tv_sec = TIMEOUT;
    timeout.tv_nsec = 0;
}

static void handle_signal(evutil_socket_t fd, short what, void *arg)
{
    struct server_context *ctx = arg;
    event_base_loopbreak(ctx->base);
}

static void on_perf(evutil_socket_t fd, short event, void *arg) {
    struct server_context *ctx = arg;
    printf("%4d %8d\n", ctx->at_second++, ctx->message_per_second);
    ctx->message_per_second = 0;
}

static void __attribute__((unused))
print_addr(struct sockaddr_in* s)
{
    printf("address %s, port %d\n", inet_ntoa(s->sin_addr), ntohs(s->sin_port));
}

static void respond(evutil_socket_t fd, int iov_len) {
    int retval = sendmmsg(fd, msgs, iov_len, 0);
    if (retval == -1)
        perror("sendmmsg()");
}

static void on_read(evutil_socket_t fd, short event, void *arg) {
    struct server_context *ctx = arg;

    int retval;
    int i;
    retval = recvmmsg(fd, msgs, VLEN, 0, &timeout);
    if (retval == -1) {
        perror("recvmmsg()");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < retval; i++) {
        bufs[i][msgs[i].msg_len] = 0;
        // print_addr(&remotes[i]);
    }

    respond(fd, retval);
    ctx->message_per_second += retval;
}

static int create_server_socket(int port) {
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
        perror("create socket server");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons(port);
    int res = bind(s, (struct sockaddr*)&addr, sizeof(addr));
    if (res < 0)
        perror("bind socket");

    return s;
}

static void subcribe_to_multicast_group(char *group, int sockfd)
{
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0) {
        perror("IP_ADD_MEMBERSHIP");
        exit(EXIT_FAILURE);
    }
}


/******************************************************************
 * Checks if specified IP is multicast IP.
 *
 * Returns 0 if specified IP is not multicast IP, else non-zero.
 *
 * Parameters:
 * ip IP to check for multicast IP, stored in network byte-order.
 ******************************************************************/
static int net_ip__is_multicast_ip(char *ip_address)
{
    in_addr_t ip_in_addr = inet_addr(ip_address);
    char *ip = (char *)&ip_in_addr;
    int i = ip[0] & 0xFF;

    if(i >=  224 && i <= 239){
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct server_context ctx;
    ctx.base = event_base_new();
    ctx.at_second = 0;
    ctx.message_per_second = 0;
    int sock = create_server_socket(12345);
    evutil_make_socket_nonblocking(sock);

    if (argc > 1) {
        if (net_ip__is_multicast_ip(argv[1])) {
            subcribe_to_multicast_group(argv[1], sock);
        }
    }

    init_msghdr();

    struct event *ev_read, *ev_perf, *ev_signal;
    ev_read = event_new(ctx.base, sock, EV_READ|EV_PERSIST, on_read, &ctx);
    event_add(ev_read, NULL);

    ev_perf = event_new(ctx.base, -1, EV_TIMEOUT|EV_PERSIST, on_perf, &ctx);
    struct timeval one_second = {1, 0};
    event_add(ev_perf, &one_second);

    ev_signal = evsignal_new(ctx.base, SIGINT|SIGTERM, handle_signal, &ctx);
    evsignal_add(ev_signal, NULL);
    event_base_priority_init(ctx.base, 4);
    event_priority_set(ev_signal, 0);
    event_priority_set(ev_perf, 2);
    event_priority_set(ev_read, 3);
    event_base_dispatch(ctx.base);

    event_free(ev_read);
    event_free(ev_perf);
    event_free(ev_signal);
    event_base_free(ctx.base);

    return 0;
}