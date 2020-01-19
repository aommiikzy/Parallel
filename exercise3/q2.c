#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
        int A[100], myid, numprocs, i, chunksize = 100/4;
        int input, local_A[chunksize], local_count = 0, count;
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &myid);

        if (myid==0) {
                srand(1234); /* Set random seed */
                for (i=0; i<100; i++) {
                        A[i] = rand()%1000; /* Set each element randomly to 0-999 */
                        //printf("[%d] - %d\n",myid,A[i]);
                }
		
		printf("Enter the number: ");
		fflush(stdout);
		scanf("%d", &input);
        }
	MPI_Bcast(&input, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(A, chunksize, MPI_INT, local_A, chunksize, MPI_INT, 0, MPI_COMM_WORLD);

	for(i=0; i<chunksize; i++){
		if(local_A[i] < input) { 
			local_count ++; 
			printf("%d ",local_A[i]);
		}
	}
	printf("\n");

	MPI_Reduce(&local_count, &count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if(myid==0) printf("process[%d] gets all counts = %d\n", myid, count);

			
 MPI_Finalize();
}


