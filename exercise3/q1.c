#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
        
        int A[100];
        int B[25];
        int i, rank, size, numprocs;
        int localMax = -1, localMin = 1000, localSum = 0, max, min, sum;

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (rank==0) {
                srand(1234); /* Set random seed */
                for (i=0; i<100; i++) {
                        A[i] = rand()%1000; /* Set each element randomly to 0-999 */
                        //printf("[%d] - %d\n",myid,A[i]);
                }
        }

        MPI_Scatter(A, 25, MPI_INT, B, 25, MPI_INT, 0, MPI_COMM_WORLD);

        /* min, max, sum(average) */
        for(i=0; i<25; i++) {
                //printf("[%d] - %d\n",myid,local_A[i]);
                if(B[i] < localMin) localMin = B[i];
                if(B[i] > localMax) localMax = B[i];
                localSum += B[i];
        }

        //printf("min = %d, max = %d, sum = %d\n", min, max, sum);

        MPI_Reduce(&localMin, &min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD); /* min */
        MPI_Reduce(&localMax, &max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD); /* max */
        MPI_Reduce(&localSum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); /* sum (average) */

        if (rank==0){
                printf("process[%d] now has min = %d, max = %d, average = %d\n", rank, min, max, sum/100);
        }
        MPI_Finalize();
}
