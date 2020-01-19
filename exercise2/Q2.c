#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]){
    int rank, size;
    int channel;
    int number;
    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    channel = 1;
    if(rank == 0){
        number = 5;
        MPI_Send(&number,1,MPI_INT,1,channel,MPI_COMM_WORLD);
        printf("Process 0 send %d\n\n", number);
    }

    if(rank == 1){
        MPI_Recv(&number,1,MPI_INT,0,channel,MPI_COMM_WORLD, &status);
        number = number * number;
        printf("Process 1 receive: %d\n", number);
    }
    MPI_Finalize();
}
