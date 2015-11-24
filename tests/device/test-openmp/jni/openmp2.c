#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>
#if !__APPLE__
#include <sys/sysconf.h>
#endif

int main ()
{
    int nthreads, tid;
    printf("SC_NPROCESSORS_ONLN: %ld\n", sysconf (_SC_NPROCESSORS_ONLN));
    printf("SC_NPROCESSORS_CONF: %ld\n", sysconf (_SC_NPROCESSORS_CONF));
  #pragma omp parallel default(shared) private(nthreads, tid)
    /* Fork a team of threads giving them their own copies of variables */
    {
      /* Obtain thread number */
        tid = omp_get_thread_num();
        printf("Hello World from thread = %d\n", tid);
      /* Only master thread does this */
        if (tid == 0)
        {
            nthreads = omp_get_num_threads();
            printf("Number of threads = %d\n", nthreads);
        }
    }  /* All threads join master thread and disband */

  return 0;
}
