#include <stdio.h>
#include <stdlib.h>
int main() {
   int i =0;
   int A[100];
   srand(1234);
   for (i=0; i < 100; i++)
   {
       A[i] = rand()%1000;
       if(A[i]>=500)
       {
           printf("%d\n",A[i]);
       }

   }
}
