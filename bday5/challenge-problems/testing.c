
#include <stdio.h>
#include <stdlib.h>
#include "testing.h"

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KNRM  "\x1B[0m"

/* function to test, the matrix is sorted or not. */
int test_sorting(void *base, int size)
{
  double *arr = (double *) base;
  printf("Testing Marix sort result......\n");
  for(int i=1;i<size;i++)
  {
    if(arr[i] < arr[i-1])
    {
      printf("Sorting Test : %sFAILED\n", KRED);
      printf("%s", KNRM);
      return 0;
    }
  }
  printf("Sorting Test : %sPASSED\n", KGRN);
  printf("%s", KNRM);
  return 1;
}

/*function to test matrix multiplication is done or not. */
int test_mat_multipy(void *mat_a, void *mat_b, void *mat_c, int size)
{

  //Type conversion generic type to requred data type
  double *a = (double *) mat_a;
  double *b = (double *) mat_b;
  double *c = (double *) mat_c;
  double val;
  int i,j,k;

  printf("Testing Multiplication result......\n");
  /* Matrix multiplication test starts */
  for(i=0;i<size;i++)
  {
      for(j=0;j<size;j++)
      {
          val=0;
          for(k=0;k<size;k++)
          {
              val = val + a[i*size + k]*b[k*size + j];
          }
          if(val != c[i*size + j])
          {
            printf("%lf %lf\n", val, c[i*size +j]);
            printf("Multiplication Test : %sFAILED\n", KRED);
            printf("%s", KNRM);
            return 0;
          }
      }
  }
  /* Matrix multiplication test completed */

  printf("Multiplication Test : %sPASSED\n", KGRN);
  printf("%s", KNRM);
  return 1;

}
