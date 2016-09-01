CC=g++
CFLAGS=-c -Wall
LDFLAGS=-L$(CURDIR)/lib
INCLUDES=-I$(CURDIR)/include
LDLIBS=-lpaxos
SOURCES=paxos_learner.c
EXECUTABLE=learner

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) -o $(EXECUTABLE) $< $(INCLUDES)  $(LDFLAGS) $(LDLIBS)

clean:
	rm $(EXECUTABLE)
