
#include <stdio.h>
#include <time.h>

#define SIZE 1<<10
#define scalar 2.0

int main()
{

  clock_t t_mat;
  int i,j,k;
  double time_taken;

  static double a[SIZE][SIZE];
  static double b[SIZE][SIZE];
  static double c[SIZE][SIZE];

  for(i=0;i<SIZE;i++)   //initialize a and b matrix.
  {
      for(j=0;j<SIZE;j++)
      {
          a[i][j] = i*j;
          b[i][j] = scalar;
      }
  }

  t_mat = clock();

  for(i=0;i<SIZE;i++)   //Addition of a and b matrix.
  {
      for(j=0;j<SIZE;j++)
      {
          c[i][j] = a[i][j] + b[i][j];
      }
  }

  t_mat = clock() - t_mat;
  time_taken = ((double)t_mat)/CLOCKS_PER_SEC;
  printf("[Add] the time taken in seconds is %lf\n",time_taken);
  return 0;

}
