#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]){
    MPI_Status status;
    int i;
    int size;
    int rank;
    int numbers[10];
    int result[10];
    srand(1234);
    for( i = 0 ; i < 10; i++){
        numbers[i] = rand() % 1000;
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        if(rank == 0){
            MPI_Send(&numbers, 10, MPI_INT, 1, 123, MPI_COMM_WORLD);
            MPI_Recv(&result, 10, MPI_INT, 1, 123, MPI_COMM_WORLD,&status);
            printf("Result From process 1\n");
            for( i = 0; i < 10; i++){
                printf("%d\n", result[i]);
            }
        }else if(rank == 1){
            MPI_Recv(&numbers, 10, MPI_INT, 0, 123, MPI_COMM_WORLD,&status);
            for( i = 0 ; i <10;i++){
                numbers[i] = numbers[i] * 10;
            }
            MPI_Send(&numbers, 10, MPI_INT, 0, 123, MPI_COMM_WORLD);
        }
    MPI_Finalize();
}
