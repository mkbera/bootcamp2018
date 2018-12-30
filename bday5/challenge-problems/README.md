# BOOTCAMP-2018@DAY-5: The Game of Speedups

## Writing Cache Efficient Code 

----------------------------------------------------------------------------------------------------------------------

### Challenge-problems [Real boot camp begins now, deadline 5.30PM]:

The goal of this challenge is to optimize two C functions: matrix multiplication and merge-sort for the underlying cache hierarchy. This folder contains the challenge-problems for the **BOOTCAMP-2018@DAY-5**. Compile the code using **make**. Do not change the makefile or any header-file(s).

---------------------------------------------------------------------------------------------------------------------

**[TASK A: 35 points, Surprise yourself: I can get you 10X or more speedup :)]** In matrix.c, write your cache optimized code for matrix multiplication in function 
**mat_multiply_opt(void *mat_a, void *mat_b, void *mat_c, int size, int tile)**. 
You will find a place that says  //write your code here 
What are you waiting for :) Write your code :)

----------------------------------------------------------------------------------------------------------------------

**[TASK B: 45 points, Surprise yourself: I can get you 100X or more speedup]** In mergesort.c, write your cache optimized code for merge sort in function **sort2D_OPT(void *base, int m, int n, int tile)**.
You will find a place that says  //write your code here 
You know what you have to do now :)  

----------------------------------------------------------------------------------------------------------------------
**[TASK C: 20 points, surprise me in terms of speedup!!]**

The top three speedups(**TASK A + TASK B**) will get 20 points.  shout “oh Captain My Captain” once you think you will be in top three. 

What!!!, did you just say that you got 10000X speedup? Awesome!! May be I am hallucinating :)

After modifying your functions (for TASK A and TASK B), type **make** to **re-compile* the code to reflect your changes in the binary files. After recompilation, Type **./mat-opt** in terminal to run your optimized code. It will show the execution times (you can see the speedup too) for both the matrix multiplication and the merge sort.

----------------------------------------------------------------------------------------------------------------------
We will evaluate **TASK A**, **TASK B**, and **TASK C** independently based on the speedup and the L3 cache size. Run your code three times and convince us that your speedup is not an outlier. So try to get the best speedup.

Canvas submission: zip matrix.c and mergesort.c, and then submit a zip file. 
