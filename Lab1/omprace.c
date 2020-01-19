#include <omp.h>
#include <stdio.h>
int main() {
   int x = 0;
int i;
#pragma omp parallel private(i) shared(x) num_threads(4)
    {
      for (i=0; i < 10000; i++)
        #pragma omp critical
          x = x + 1;
    } /* end of parallel region */
    printf("%d\n", x);
// The correct output is 40000

}
