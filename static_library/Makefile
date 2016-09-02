CC=g++
CFLAGS=-c -Wall
LDFLAGS=-L$(CURDIR)/lib
INCLUDES=-I$(CURDIR)/include
LDLIBS=-lpaxos
SOURCES=paxos_learner.c
EXECUTABLE=learner

all: $(EXECUTABLE) acceptor

$(EXECUTABLE): $(SOURCES)
	$(CC) -o $(EXECUTABLE) $< $(INCLUDES)  $(LDFLAGS) $(LDLIBS)

acceptor: acceptor_test.c
	$(CC) -o $@ $< $(INCLUDES)  $(LDFLAGS) $(LDLIBS)

clean:
	rm $(EXECUTABLE) acceptor
