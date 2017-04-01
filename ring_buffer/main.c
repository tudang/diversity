#include <stdio.h>
#include "ring_buffer.h"

int test_ringbuf_int()
{
    int i;
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8};
    int array_size = sizeof(array)/sizeof(array[0]);
    struct ringbuf_t* buffer = NewRingBuffer(7);

    for (i = 0; i < array_size; i++) {
        if (PushToQueue(&array[i], buffer) != 0)
            break;
    }

    PrintBuffer(buffer);

    PushToQueue(&array[3], buffer);
    PrintBuffer(buffer);

    int *el = RetrieveFromQueue(buffer);
    if (el != NULL) {
        printf("%d\n", *el);
    }

    PushToQueue(&array[3], buffer);
    PrintBuffer(buffer);

    el = RetrieveFromQueue(buffer);
    if (el != NULL) {
        printf("%d\n", *el);
    }

    PushToQueue(&array[7], buffer);
    PrintBuffer(buffer);

    FreeRingBuffer(buffer);

    return 0;
}

void PrintCharBuffer(struct ringbuf_t* buffer)
{
    int i;
    for (i = 0; i < buffer->active; i++) {
        int idx = (buffer->start + i) % buffer->size;
        printf(" %c", *(char *)buffer->ring[idx]);
    }
    printf("\n");
}

int test_ringbuf_char()
{
    char carr[] = { 'A', 'B', 'C', 'D'};
    struct ringbuf_t* buffer = NewRingBuffer(3);

    PushToQueue(&carr[0], buffer);
    PrintCharBuffer(buffer);
    PushToQueue(&carr[1], buffer);
    PushToQueue(&carr[2], buffer);
    PrintCharBuffer(buffer);
    PushToQueue(&carr[3], buffer);
    PrintCharBuffer(buffer);
    char* el = RetrieveFromQueue(buffer);
    if (el != NULL) {
        printf("%c\n", *el);
    }
    PushToQueue(&carr[3], buffer);
    PrintCharBuffer(buffer);
    el = RetrieveFromQueue(buffer);
    if (el != NULL) {
        printf("%c\n", *el);
    }
    char F = 'F';
    PushToQueue(&F, buffer);
    PrintCharBuffer(buffer);
    FreeRingBuffer(buffer);

    return 0;
}

int main(int argc, char* argv[])
{
    test_ringbuf_int();
    // test_ringbuf_char();
    return 0;
}