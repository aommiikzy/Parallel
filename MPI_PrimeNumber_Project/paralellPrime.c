#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define BLOCK_MIN(id,p,n) ((id)*(n)/(p))
#define BLOCK_MAX(id,p,n) (BLOCK_MIN((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) (BLOCK_MAX(id,p,n)-BLOCK_MIN(id,p,n)+ 1)


int main (int argc, char *argv[])
{
     int n,i,index,k,j,count,global_count,first;
     int* listMarked;
     double t1,t2;
     int proc0_size;
     int min_value;
     int max_value;
     int size,p,id;
     MPI_Init (&argc, &argv);
     MPI_Barrier(MPI_COMM_WORLD);
     t1 = MPI_Wtime();
     MPI_Comm_rank (MPI_COMM_WORLD, &id);
     MPI_Comm_size (MPI_COMM_WORLD, &p);
      if (argc != 2)
      {
      if (!id)
      {
        printf ("Please input the range of number!!\n");
      }
      MPI_Finalize();
      exit (1);
      }
      n = atoi(argv[1]);
      // set n equal the input from user
      //increase by two because we will skip the number 0 and 1 in the first block
      min_value = 2 + BLOCK_MIN(id,p,n-1);
      max_value = 2 + BLOCK_MAX(id,p,n-1);
      // printf("Test = %d\n",BLOCK_MAX(id,p,n-1) );
      size = BLOCK_SIZE(id,p,n-1);
      proc0_size = (n-1)/p;
      // the size of process 0 will minimum because it is not contain 0
      // printf("Start K (block %d) = %d\n",id, k);
      // printf("min_value(block %d) = %d\n",id, min_value);
      // printf("max_value(block %d) = %d\n",id, max_value);
      // // printf("\n");
      // printf("size = %d\n", size);
      // printf("proc0_size = %d\n", proc0_size);
      // printf("\n");
        // printf("proc0_size = \n"m);
        if ((2 + proc0_size ) < (int) sqrt((double) n))
        {
            if (id==0) printf ("Too many processes\n");
            MPI_Finalize();
            exit (1);
        }
        // create a list mark to mark the value which is the prime number
        listMarked = malloc(sizeof(int)*size);
        if (listMarked == NULL)
        {
         printf ("Cannot allocate enough memory\n");
         MPI_Finalize();
         exit (1);
        }
        // mark  all number to be zero
        for (i = 0; i < size; i++)
        {
          listMarked[i] = 0;
          // printf("listMarked = %d\n",listMarked[i]);
        }
        // if id of that block equal 0 will set it to be 0
        if (id==0)
        {
            index = 0;
        }
        // initialize the k = 2 and it will do in this loop
        // k will set to be  a new prime until k*k more than the lastest
        // until k*k more than the the length of N
        k=2;
        // printf(" K = %d\n",k );
        do {
        /* Find the first number in block that mod k = 0 */
         // printf(" K = %d , min_value = %d \n",k,min_value );
            if (k * k > min_value)
            {
              // printf("First from 1 = %d\n",first );
            first = k * k - min_value;
            }
            else
            {
              if ((min_value % k)==0)
              {
                  first = 0;
                  // it means that first value in that block is not prime number
              }

              else first = k - (min_value % k); // printf("First from 3 = %d\n",first );
            }
            // this loop will skip the block by k and set it to not be prime number
            // because that block is +k can divided by k
            for (i = first; i < size; i += k)
            {

                listMarked[i] = 1;
                // printf("%d\n",i );
            }
            if (id==0)
            {
            // This while loop will find the next k of block 0 until listMarked is not equal 1
            while (listMarked[++index] == 1);
            k = index + 2;
            }
            // This function will broadcast the value of new k
            MPI_Bcast (&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
        } while (k * k <= n);

          // printf("block = %d\n",id);
          // printf("\n");
        // Below loop is a loop will count value in each block in each process
        // if it equal 0 it will count the number of prime number
        count = 0;
        for (i = 0; i < size; i++)
        {
            if (!listMarked[i])
            {

                // printf("%d \n",min_value+i);
                count++;
            }
        }
        //This function will use operation sum to increase the number of globol count
        MPI_Reduce (&count, &global_count, 1, MPI_INT, MPI_SUM,0, MPI_COMM_WORLD);
        // if if equal 0 will quit and show the result.
        if (id==0)
        {
          // t2 = MPI_Wtime();
            t2 = MPI_Wtime();
           printf ("Total time: %f seconds to execute\n", t2-t1);
              // printf ("Total time2: %10.6f\n", elapsed_time);
        printf ("%d primes to %d\n",global_count, n);
        }
        MPI_Finalize ();
        }
