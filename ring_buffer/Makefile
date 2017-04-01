CC=gcc
IDIR=.
CFLAGS=-O3 -Wall -Wextra -Wno-unused-parameter -ggdb -g -I $IDIR
SOURCES=main.c ring_buffer.c

objects=$(SOURCES:.c=.o)
EXECUTABLE=main

$(EXECUTABLE): $(objects)
	$(CC) -o $(EXECUTABLE) $(LDFLAGS) $(objects)

%.c.o:


.PHONY : clean
clean:
	rm -f $(EXECUTABLE) $(objects)