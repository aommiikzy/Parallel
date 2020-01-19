#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]){
    int size;
    int rank;
    int i;
    int j;
    MPI_Status status;
    int data[8][8];
    int receiveData[8];
    int sumOfEach;
    int sum;
    srand(1238);
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(rank == 0){
        sum = 0;
        for(i = 0 ; i < 8; i++){
            for(j = 0; j < 8; j++){
                data[i][j] = i + j;
            }
        }

        for(i = 1; i < 5; i++){
            MPI_Send(&data[i-1], 8, MPI_INT, i, 232, MPI_COMM_WORLD);
        }
        for(i = 1; i < 5; i++){
            MPI_Recv(&sumOfEach, 8, MPI_INT, i, 232, MPI_COMM_WORLD, &status);
            sum = sum + sumOfEach;
        }
        printf("%d\n", sum);

    }
    else{
        int sumOf;
        MPI_Recv(&receiveData, 8, MPI_INT, 0, 232, MPI_COMM_WORLD, &status);
        for(i = 0 ; i < 8; i++){
            sumOf = sumOf + receiveData[i];
        }
        MPI_Send(&sumOf, 8, MPI_INT, 0, 232, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}
