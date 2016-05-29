#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>

void send_to_addr(int fd, struct sockaddr_in *serverAddr);

int
timespec_diff(struct timespec *result, struct timespec *end,struct timespec *start)
{
  result->tv_sec = end->tv_sec - start->tv_sec;
  result->tv_nsec = end->tv_nsec - start->tv_nsec;

  /* Return 1 if result is negative. */
  return end->tv_sec < start->tv_sec;
}


void handle_signal(evutil_socket_t fd, short what, void *arg)
{
    printf("Caught SIGINT\n");
    struct event_base *base = arg;
    event_base_loopbreak(base);
}

void on_read(evutil_socket_t fd, short event, void *arg) {
    struct sockaddr_in remote;
    socklen_t addrlen;
    struct timespec start;
    int n = recvfrom(fd, &start, sizeof(start), 0, (struct sockaddr*)&remote, &addrlen);
    if (n < 0)
        perror("recvfrom");
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec result;
    timespec_diff(&result, &end, &start);
    printf("%ld.%.9ld\n", result.tv_sec, result.tv_nsec);

    send_to_addr(fd, &remote);
}

int new_dgram_socket() {
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("New DGRAM socket");
        return -1;
    }
    return s;
}

void send_to_addr(int fd, struct sockaddr_in *serverAddr) {
    socklen_t addr_size = sizeof (*serverAddr);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    int msg_size = sizeof ts;   
    int n = sendto(fd , &ts, msg_size, 0, (struct sockaddr *)serverAddr, addr_size);
    if (n < 0) {
        perror("sendto");
        printf("address %s, port %d\n", inet_ntoa(serverAddr->sin_addr),
                ntohs(serverAddr->sin_port));
    }
}

void send_a_packet(int fd, char *dest) {
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = inet_addr(dest);
    send_to_addr(fd, &serverAddr);
}

int main(int argc, char *argv[])
{
    struct event_base *base = event_base_new();

    int sock = new_dgram_socket();
    evutil_make_socket_nonblocking(sock);
    struct event *ev_read, *ev_signal;
    ev_read = event_new(base, sock, EV_READ|EV_PERSIST, on_read, base);
    event_add(ev_read, NULL);

    ev_signal = evsignal_new(base, SIGINT, handle_signal, base);
    evsignal_add(ev_signal, NULL);

    send_a_packet(sock, argv[1]);

    event_base_dispatch(base);

    event_free(ev_read);
    event_free(ev_signal);
    event_base_free(base);

    return 0;
}