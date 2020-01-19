#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
        int A[8][8], myid, numprocs, i, j, chunksize = 8/8;
        int local_A[chunksize*8], local_sum = 0, sum;

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &myid);

        if (myid==0) {
                srand(1234); /* Set random seed */
                for (i=0; i<8; i++) {
                        for (j=0; j<8; j++){
                                A[i][j] = rand()%1000; /* Set each element randomly to 0-999 */
                                printf("%d\t",A[i][j]);
                        }
			printf("\n");
                }
        }

        MPI_Scatter(A, chunksize*8, MPI_INT, local_A, chunksize*8, MPI_INT, 0, MPI_COMM_WORLD);

        for(i=0; i<chunksize*8; i++){
                local_sum += local_A[i];
        }

        MPI_Reduce(&local_sum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        if(myid==0) {
                printf("sum = %d\n",sum);
        }

 MPI_Finalize();
}
