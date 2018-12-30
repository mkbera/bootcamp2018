
/* Header file for matrix operations */
#ifndef H_MATRIX
#define H_MATRIX
  void mat_multiply(void *mat_a, void *mat_b, void *mat_c, int size);  //function declaration for matrix multiply.
  void mat_multiply_opt(void *mat_a, void *mat_b, void *mat_c, int size, int tile); //function declaration for matrix multiply. Need an implementation in matrix.c
#endif
