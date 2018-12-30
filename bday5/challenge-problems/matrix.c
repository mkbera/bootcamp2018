
#include "matrix.h"
#include <stdio.h>
#define min(a,b) ((a)<(b)?(a):(b)) //Macro to return small value from a and  b.
#define i_j (ti*tile+i)*size + (tj*tile+j)
#define i_k (ti*tile+i)*size + (tk*tile+k)
#define k_j (tk*tile+k)*size + (tj*tile+j)

/*function to perform matric multiplication.*/
void mat_multiply(void *mat_a, void *mat_b, void *mat_c, int size)
{
  //Type conversion generic type to requred data type
  double *a = (double *) mat_a;
  double *b = (double *) mat_b;
  double *c = (double *) mat_c;
  int i,j,k;

  /* Matrix multiplication starts */
  for(i=0;i<size;i++)
  {
      for(j=0;j<size;j++)
      {
          c[i*size + j]=0;
          for(k=0;k<size;k++)
          {
              c[ i*size + j] = c[i*size + j]+a[i*size + k]*b[k*size + j];
          }
      }
  }

  /* Matrix multiplication Completed. */
}

/*Function template to implement tiled Matrix multiplication. */
void mat_multiply_opt(void *mat_a, void *mat_b, void *mat_c, int size, int tile)
{
  double *a = (double *) mat_a;
  double *b = (double *) mat_b;
  double *c = (double *) mat_c;

  int i, j, k;
  int ti, tj, tk;

  // for (i=0; i<size; i++) {
  //   for (j=0; j<size; j++) {
  //     c[i*size + j] = 0;
  //   }
  // }

  int n_tiles = size / tile;

  // printf("n tiles %d\n", n_tiles);
  for (ti=0; ti<n_tiles; ti++) { 
    for (tj=0; tj<n_tiles; tj++) {
      for(i=0; i<tile; i++) {
        for(j=0; j<tile; j++) {
          c[i_j] = 0;
        }
      }
      for (tk=0; tk<n_tiles; tk++) {
        // now we have a tile from each matrix
        for(i=0; i<tile; i++) {
          for(j=0; j<tile; j++) {
            for(k=0; k<tile; k++) {
              c[i_j] = c[i_j] + a[i_k] * b[k_j];
            }
          }
        }

      }
    }
  }
  // if(size % tile == 0) printf("MARK\n");
  //write your code here

}
