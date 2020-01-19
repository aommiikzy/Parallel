#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
int main() {
    int i;
    int A[100];
    srand(1234); /* Set random seed */
    for (i=0; i < 100; i++){
        A[i] = rand()%1000; /* Set each element randomly to 0-999 */
    }
    int sum = 0;
    int pnum = 0;
#pragma omp parallel private(pnum, i) shared(sum) num_threads(3)
    {
#pragma omp parallel for
        for (i=0; i < 100; i++){
            pnum += A[i];
        }
#pragma omp critical
        sum += pnum;
    }
    printf("%d\n",sum);
}
