#include <stdio.h>
#include <stdlib.h>
#include "ring_buffer.h"

struct ringbuf_t* NewRingBuffer(int size)
{
    struct ringbuf_t* buffer = malloc(sizeof(struct ringbuf_t));
    buffer->ring  = malloc(sizeof(void*));
    buffer->size = size;
    buffer->start = 0;
    buffer->end = 0;
    buffer->active = 0;
    return buffer;
}

int PushToQueue(void *p, struct ringbuf_t* buffer)
{
    if (buffer->active < buffer->size)
    {
        buffer->ring[buffer->end] = p;
        buffer->end = (buffer->end + 1) % buffer->size;
        buffer->active++;
        return 0;
    } else {
        /* Overwriting the oldest. Move start to next-oldest */
        // buffer->start = (buffer->start + 1) % buffer->size;

        return ERROR_BUFFER_FULL;
    }
}

void *RetrieveFromQueue(struct ringbuf_t* buffer)
{
    void *p;

    if (!buffer->active) { return NULL; }

    p = buffer->ring[buffer->start];
    buffer->start = (buffer->start + 1) % buffer->size;

    buffer->active--;
    return p;
}

void PrintBuffer(struct ringbuf_t* buffer)
{
    int i;
    for (i = 0; i < buffer->active; i++) {
        int idx = (buffer->start + i) % buffer->size;
        printf(" %d", *(int *)buffer->ring[idx]);
    }
    printf("\n");
}

void FreeRingBuffer(struct ringbuf_t* buffer)
{
    free(buffer->ring);
    free(buffer);
}