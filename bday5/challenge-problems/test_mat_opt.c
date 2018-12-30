
#include <stdio.h>
#include <time.h>
#include "sorting.h"
#include "matrix.h"
#include "testing.h"

#define SIZE 1024
#define scalar 2.0
#define MAT_TILE 16 //represents the matrix tile size.
#define SORT_TILE 512 //represents sortng tile size.


int main()
{

  clock_t t_mat, t_sort;
  int i,j;
  double time_taken;

  static double a[SIZE][SIZE];
  static double b[SIZE][SIZE];
  static double c[SIZE][SIZE];

  for(i=0;i<SIZE;i++)   //initialize a, b and c matrix.
  {
      for(j=0;j<SIZE;j++)
      {
        a[i][j] = i*j;
        b[i][j] =  ( int) (scalar * i) % (j+1);
        c[i][j] = scalar + i*j;
      }
  }

  t_mat = clock();
  mat_multiply_opt(a, b, c, SIZE, MAT_TILE);
  t_mat = clock() - t_mat;

  if(test_mat_multipy(a, b, c, SIZE))  //checking multiplication is done properly.
  {
    time_taken = ((double)t_mat)/CLOCKS_PER_SEC;
    printf("[Mult] the time taken in seconds is %lf\n",time_taken);
  }

  // t_sort = clock();
  // sort2D_OPT(c, SIZE, SIZE, SORT_TILE);
  // t_sort = clock() - t_sort;

  // if(test_sorting(c, SIZE*SIZE)) // checking Matric c is sorted.
  // {
  //   time_taken = ((double)t_sort)/CLOCKS_PER_SEC;
  //   printf("[Sort] the time taken in seconds is %lf\n",time_taken);
  // }

  return 0;

}
