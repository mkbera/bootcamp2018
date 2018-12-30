
#include <stdio.h>
#include <stdlib.h>

#include "sorting.h"

// Merges two subarrays.
// First subarray is from[l..m]
// Second subarray is from [m+1..r]
void merge(void *base, int l, int m, int r)
{
  int i, j, k;
  int n1 = m - l + 1;
  int n2 =  r - m;
  double *arr = (double *) base;

  /* create temp arrays */
  double *L = malloc(n1*sizeof(double));
  double *R = malloc(n2*sizeof(double));

  if(L == NULL || R == NULL)
  {
    if(L != NULL)
      free(L);
    if(R != NULL)
        free(R);
    printf("No enough memory for operations.\n" );
    exit(0);
  }

  /* Copy data to temp arrays L[] and R[] */
  for (i = 0; i < n1; i++)
      L[i] = arr[l + i];
  for (j = 0; j < n2; j++)
      R[j] = arr[m + 1+ j];

  /* Merge the temp arrays back into arr[l..r]*/
  i = 0; // Initial index of first subarray
  j = 0; // Initial index of second subarray
  k = l; // Initial index of merged subarray
  while (i < n1 && j < n2)
  {
      if (L[i] <= R[j])
      {
          arr[k] = L[i];
          i++;
      }
      else
      {
          arr[k] = R[j];
          j++;
      }
      k++;
  }

  /* Copy the remaining elements of L[], if there
     are any */
  while (i < n1)
  {
      arr[k] = L[i];
      i++;
      k++;
  }

  /* Copy the remaining elements of R[], if there
     are any */
  while (j < n2)
  {
      arr[k] = R[j];
      j++;
      k++;
  }

  if(L != NULL)
    free(L);
  if(R != NULL)
      free(R);
}

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(void *base, int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        mergeSort(base, l, m);
        mergeSort(base, m+1, r);

        merge(base, l, m, r);
    }
}

/* UTILITY FUNCTIONS */

/* Wrapper function to sort the matrix */

void sort2D(void *base, int m, int n, int tile)
{

  int i, nTiles;
  nTiles = m*n / tile;
  nTiles = (m*n)%tile == 0 ? nTiles : nTiles + 1;
  for(i=0; i<nTiles; i+=1)
  {
    if((m*n - i*tile) < tile)   //checking if the size of last chunk is smaller then the tile size.
      mergeSort(base, i*tile, m*n-1);
    else
      mergeSort(base,i*tile,((i+1)*tile - 1));
  }

  //merging the sorted result.
  for(i=0; i<nTiles-1; i+=1)
  {
     if((m*n - (i+1)*tile) < tile)
      merge(base,0,((i+1)*tile -1), m*n-1);
     else
      merge(base,0,((i+1)*tile -1), ((i+2)*tile -1));
  }
}

/* Function template to implement optimized sort*/
void sort2D_OPT(void *base, int m, int n, int tile){

  // Write your code here.

  // You are allowed to modify any functions and can
  // introduce new function only in this file.

  // Things not allowed: Signature modification of existing function and
  // any modification in header files


}

/* Function to print 2D matrix */
void print2D(void *base, int m, int n)
{
  int i,j;
  double *intArra = (double *) base;
  for(i=0;i<m*n;)
  {
    printf("%lf\t", intArra[i]);
    if((++i)%m == 0)
      printf("\n");
  }
}
