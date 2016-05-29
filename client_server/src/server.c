#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <signal.h>


struct server_context {
    struct event_base *base;
    int at_second;
    int message_per_second;
};

void handle_signal(evutil_socket_t fd, short what, void *arg)
{
    printf("Caught SIGINT\n");
    struct server_context *ctx = arg;
    event_base_loopbreak(ctx->base);
}

void on_perf(evutil_socket_t fd, short event, void *arg) {
    struct server_context *ctx = arg;
    printf("%4d %8d\n", ctx->at_second++, ctx->message_per_second);
    ctx->message_per_second = 0;
}

void on_read(evutil_socket_t fd, short event, void *arg) {
    struct server_context *ctx = arg;
    struct sockaddr_in remote;
    socklen_t addrlen = sizeof(remote);
    char buf[1024];
    int recv_bytes = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&remote, &addrlen);
    if (recv_bytes < 0) {
        perror("recvfrom");
        return;
    }

    ctx->message_per_second++;

    int send_bytes = sendto(fd, buf, recv_bytes, 0, (struct sockaddr*)&remote, addrlen);
    if (send_bytes < 0) {
        perror("sendto");
        return;
    }
}

int create_server_socket(int port) {
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



int main(int argc, char *argv[])
{
    struct server_context ctx;
    ctx.base = event_base_new();
    ctx.at_second = 0;
    ctx.message_per_second = 0;
    int sock = create_server_socket(12345);
    evutil_make_socket_nonblocking(sock);
    struct event *ev_read, *ev_perf, *ev_signal;
    ev_read = event_new(ctx.base, sock, EV_READ|EV_PERSIST, on_read, &ctx);
    event_add(ev_read, NULL);

    ev_perf = event_new(ctx.base, -1, EV_TIMEOUT|EV_PERSIST, on_perf, &ctx);
    struct timeval one_second = {1, 0};
    event_add(ev_perf, &one_second);

    ev_signal = evsignal_new(ctx.base, SIGINT, handle_signal, &ctx);
    evsignal_add(ev_signal, NULL);

    event_base_dispatch(ctx.base);

    event_free(ev_read);
    event_free(ev_perf);
    event_free(ev_signal);
    event_base_free(ctx.base);

    return 0;
}