#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
        int A[100], B[100], C[100];
	int psnum, id, i;
        int chunksize = 100/2; 
	int X[chunksize], Y[chunksize], Z[chunksize];

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &psnum);
        MPI_Comm_rank(MPI_COMM_WORLD, &id);

        if (id==0) {
                srand(1234); /* Set random seed */
                for (i=0; i<100; i++) {
                        A[i] = rand()%1000; /* Set each element randomly to 0-999 */
                }
                srand(1234); /* Set random seed */
                for (i=0; i<100; i++) {
                        B[i] = rand()%1000; /* Set each element randomly to 0-999 */
                }
        }

        MPI_Scatter(A, chunksize, MPI_INT, X, chunksize, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(B, chunksize, MPI_INT, Y, chunksize, MPI_INT, 0, MPI_COMM_WORLD);

        for(i=0; i<chunksize; i++){
                Z[i] = A[i] + B[i];
        }

        MPI_Gather(Z, chunksize, MPI_INT, C, chunksize, MPI_INT, 0, MPI_COMM_WORLD);
	
        if(id==0) {
                for (i=0; i<100; i++) {
                        printf("C[%d] = %d \n",i,C[i]);
                }
        }

 MPI_Finalize();
}

