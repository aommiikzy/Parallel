#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
  a:while (1) {


  long n = 10000000;
  printf("Input the length of number\n");
  scanf("%ld",&n);
  if(n==-1)
  {
    break;
  }
  // if(n >= 10000000)
  // {
  //   printf("Your input number is very large\n");
  //   goto a;
  // }
  clock_t t;
  t = clock();
  int i,j;
  int list[n];
  int mark[n];
  int find;
  int k=2;
  for(i=2;i<=n;i++)
  {
    list[i]=i;
    mark[i]=i;
  }
  int sum=0;
  while((k*k)<=n)
  {
    for(j=k*k;j<=n;j++)
    {
      if((j%k)==0)
      {
        mark[j]=0;
      }
    }
    for(i=2;i<=n;i++)
    {
      if(mark[i]>k)
      {
        k=i;
        // printf("New k = %d\n",k);
        break;
      }
    }
  }
  for(i=2;i<=n;i++)
  {
    if(mark[i]!=0)
    {
    sum++;
    printf("%d\n",mark[i]);
    }
  }
  t = clock() - t;
   double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
    printf("All answer = %d\n",sum);
     printf("Time =  %f seconds to execute \n", time_taken);

}
return 0;
}
