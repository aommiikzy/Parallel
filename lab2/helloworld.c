#include "mpi.h"
#include <stdio.h>
int main(int argc,char *argv[] )
{
 int rank, size;
 char hostname[MPI_MAX_PROCESSOR_NAME];
 int resultlen;
 MPI_Init( &argc, &argv );
 MPI_Comm_rank( MPI_COMM_WORLD, &rank );
 MPI_Comm_size( MPI_COMM_WORLD, &size );
 MPI_Get_processor_name(hostname,&resultlen);
 printf( "Hello! I'm %d of %d running on %s\n",rank,size,hostname);
 MPI_Finalize();
 return 0;
} 
