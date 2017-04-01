#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#define BUFFER_SIZE 8

#define ERROR_BUFFER_FULL 1

struct ringbuf_t {
    void **ring;
    int size;
    int start;
    int end;
    int active;
};

struct ringbuf_t* NewRingBuffer(int size);
int PushToQueue(void *p, struct ringbuf_t* buffer);
void *RetrieveFromQueue(struct ringbuf_t* buffer);
void PrintBuffer(struct ringbuf_t* buffer);
void FreeRingBuffer(struct ringbuf_t* buffer);
#endif
