#include <string.h>
#include "functions.h"

int parse_lineinput(char *lineinput, char **array, int *count)
{
  int i = 0;
  *count = 0;
  char *p = strtok (lineinput, " ");

  while (p != NULL)
  {
      array[i++] = p;
      p = strtok (NULL, " ");
  }
  *count = i;

  return 0;
}
