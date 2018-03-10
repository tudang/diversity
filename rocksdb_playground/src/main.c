#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<readline/readline.h>
#include<readline/history.h>

#include "functions.h"

struct rocksdb_params rocks;

#define MAX_NUM_ELEMENTS 10

int main(){

    init_rocksdb();

    char prompt[] = "p4xos> ";

    char* lineinput;
    char *array[MAX_NUM_ELEMENTS];
    int count;

    while(1)
    {
      lineinput = readline(prompt);
      add_history(lineinput);
      if (lineinput == NULL) {
        break;
      }

      parse_lineinput(lineinput, array, &count);
      handle_requests(array, count);

      free(lineinput);
    }

    deconstruct();
    return 0;
}
