#include <stdio.h>
#include <stdlib.h>
int main() {
   int i =0;
   int A[100];
   srand(1234);
   int sum = 0;
   int max_value = 0;
   #pragma omp parallel for reduction(max:max_value)
   for (i=0; i < 100; i++)
   {
       A[i] = rand()%1000;
       
       if(A[i]>max_value)
       {
           max_value = A[i];
       }
   }

//       printf("%d\n",A[i]);
   
   printf("max = %d\n",max_value);
}
