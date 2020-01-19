#include <stdio.h>
#include <stdlib.h>
int main()
{
    int A[100];
    srand(1234);
    int i =0;
    for (i=0; i < 100; i++)
    {
        A[i] = rand()%1000;
        printf("%d\n",A[i] );
    }

}
