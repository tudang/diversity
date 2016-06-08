#include <stdio.h>
#include <string.h>

#define BUFSIZE 1024
#define TIMES   1000000

void __attribute__ ((noinline)) func2(char *buf, int size) {
    memset(buf, 'c', size - 2);
    buf[size-1] = '\0' ;
}

void __attribute__ ((noinline)) stack_allocation() {
    char buf[BUFSIZE];
    int i;
    for (i = 0; i < TIMES; i++)
        func2(buf, BUFSIZE);
}

void __attribute__ ((noinline)) heap_allocation(void *arg, int size) {
    char *buf = arg;
    int i;
    for (i = 0; i < TIMES; i++)
        func2(buf, size);
}

int main(int argc, char *argv[]) {
    char buf[BUFSIZE];
    heap_allocation(buf, BUFSIZE);
    stack_allocation();
    return 0;
}