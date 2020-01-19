#include <stdio.h>
#include <stdlib.h>
int main() {
   int i =0;
   int A[100];
   srand(1234);
   int sum = 0;
   #pragma omp parallel for reduction(+:sum)
   for (i=0; i < 100; i++)
   {
       A[i] = rand()%1000;
       sum = sum + A[i];
//       printf("%d\n",A[i]);
   }
   printf("sum = %d\n",sum);
}
