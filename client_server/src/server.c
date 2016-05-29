#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

void handle_signal(evutil_socket_t fd, short what, void *arg)
{
    printf("Caught SIGINT\n");
    struct event_base *base = arg;
    event_base_loopbreak(base);
}

void on_read(evutil_socket_t fd, short event, void *arg) {
    struct sockaddr_in remote;
    socklen_t addrlen;
    struct timespec buf;
    int n = recvfrom(fd, &buf, sizeof(buf), 0, (struct sockaddr*)&remote, &addrlen);
    if (n < 0)
        perror("recvfrom");
    n = sendto(fd, &buf, sizeof(buf), 0, (struct sockaddr *)&remote, addrlen);
    if (n < 0)
        perror("sendto");
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
    struct event_base *base = event_base_new();

    int sock = create_server_socket(12345);
    evutil_make_socket_nonblocking(sock);
    struct event *ev_read, *ev_signal;
    ev_read = event_new(base, sock, EV_READ|EV_PERSIST, on_read, base);
    event_add(ev_read, NULL);

    ev_signal = evsignal_new(base, SIGINT, handle_signal, base);
    evsignal_add(ev_signal, NULL);

    event_base_dispatch(base);

    event_free(ev_read);
    event_free(ev_signal);
    event_base_free(base);

    return 0;
}