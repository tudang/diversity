#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#define NS_PER_S 1000000000

struct client_context {
    struct event_base *base;
    struct sockaddr_in server_addr;
};

static struct {
    uint64_t nb_packets;
    uint64_t latency;
} stat;

void send_to_addr(int fd, struct client_context *ctx);

int
timespec_diff(struct timespec *result, struct timespec *end,struct timespec *start)
{
    if (end->tv_nsec < start->tv_nsec) {
        result->tv_nsec =  NS_PER_S + end->tv_nsec - start->tv_nsec;
        result->tv_sec = end->tv_sec - start->tv_sec - 1;
    } else {
        result->tv_nsec = end->tv_nsec - start->tv_nsec;
        result->tv_sec = end->tv_sec - start->tv_sec;
    }
  /* Return 1 if result is negative. */
  return end->tv_sec < start->tv_sec;
}

void on_perf(evutil_socket_t fd, short what, void *arg)
{
    float avg_us = (float)stat.latency / stat.nb_packets / 1000;
    printf("%6.2f\n", avg_us);
    stat.latency = 0;
    stat.nb_packets = 0;
}


void handle_signal(evutil_socket_t fd, short what, void *arg)
{
    struct client_context *ctx = arg;
    event_base_loopbreak(ctx->base);
}

void on_read(evutil_socket_t fd, short event, void *arg) {
    struct client_context *ctx = arg;
    struct sockaddr_in remote;
    socklen_t addrlen = sizeof(remote);
    struct timespec start;
    int n = recvfrom(fd, &start, sizeof(start), 0, (struct sockaddr*)&remote, &addrlen);
    if (n < 0)
        perror("recvfrom");
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec result;
    if ( timespec_diff(&result, &end, &start) < 1) {
        // printf("%ld.%09ld\n", result.tv_sec, result.tv_nsec);
        stat.latency += result.tv_sec*NS_PER_S + result.tv_nsec;
        stat.nb_packets++;
    }


    send_to_addr(fd, ctx);
}

int new_dgram_socket() {
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("New DGRAM socket");
        return -1;
    }
    return s;
}

void send_to_addr(int fd, struct client_context *ctx) {
    socklen_t addr_size = sizeof (ctx->server_addr);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    int msg_size = sizeof ts;   
    int n = sendto(fd , &ts, msg_size, 0, (struct sockaddr *)&ctx->server_addr, addr_size);
    if (n < 0) {
        perror("sendto");
    }
}


int main(int argc, char *argv[])
{
    struct hostent *server;
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host as %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    struct client_context ctx;

    memset(&ctx.server_addr, 0, sizeof ctx.server_addr);
    ctx.server_addr.sin_family = AF_INET;
    memcpy((char *)&(ctx.server_addr.sin_addr.s_addr), (char *)server->h_addr, server->h_length);
    ctx.server_addr.sin_port = htons(12345);

    // printf("address %s, port %d\n", inet_ntoa(ctx.server_addr.sin_addr),ntohs(ctx.server_addr.sin_port));

    ctx.base = event_base_new();
    int sock = new_dgram_socket();
    evutil_make_socket_nonblocking(sock);

    struct event *ev_read, *ev_signal, *ev_perf;
    ev_read = event_new(ctx.base, sock, EV_READ|EV_PERSIST, on_read, &ctx);
    event_add(ev_read, NULL);

    ev_perf = event_new(ctx.base, -1, EV_TIMEOUT|EV_PERSIST, on_perf, NULL);
    struct timeval one_second = {1, 0};
    event_add(ev_perf, &one_second);

    ev_signal = evsignal_new(ctx.base, SIGINT|SIGTERM, handle_signal, &ctx);
    evsignal_add(ev_signal, NULL);

    send_to_addr(sock, &ctx);

    event_base_dispatch(ctx.base);

    event_free(ev_read);
    event_free(ev_signal);
    event_free(ev_perf);
    event_base_free(ctx.base);

    return 0;
}