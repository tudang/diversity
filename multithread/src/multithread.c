#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include <signal.h>
#include <sys/sysinfo.h>

int stopped;
/*Signal handler to return from pause*/
void sig_handler(int signo)
{
  stopped = 1;
}


// A normal C function that is executed as a thread
// when its name is specified in pthread_create()
void *yesThread(void *vargp)
{
  FILE *devNull = freopen("/dev/null", "w", stdout);
  while(!stopped) {
    fprintf(devNull, "y\n");
  }

  return NULL;
}

int main()
{
  stopped = 0;
  signal(SIGINT, sig_handler);
  signal(SIGKILL, sig_handler);
  int n_procs = get_nprocs();
  int i;
  pthread_t thread_id[n_procs];
  for (i=0; i < n_procs; i++)
    pthread_create(&thread_id[i], NULL, yesThread, NULL);

  for (i=0; i < n_procs; i++) {
    pthread_join(thread_id[i], NULL);
  }

  return 0;
}
