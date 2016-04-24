CC=gcc
CFLAGS=-O3 -Wall -Wextra -Wno-unused-parameter -ggdb -g -pthread

all: sender receiver

sender: udpsender.c net.c
	$(CC) $(CFLAGS) udpsender.c net.c -o sender

receiver: udpreceiver1.c net.c
	$(CC) $(CFLAGS) udpreceiver1.c net.c -o receiver
