#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<string.h>
#include<pthread.h>
#include<math.h>
#include<assert.h>

#define SEED 0x7457

#define MAX_THREADS 64
#define USAGE_EXIT(s) do{ \
                             printf("Usage: %s <# of elements> <# of threads> \n %s\n", argv[0], s); \
                            exit(-1);\
                    }while(0);

#define TDIFF(start, end) ((end.tv_sec - start.tv_sec) * 1000000UL + (end.tv_usec - start.tv_usec))

struct thread_param{
                       pthread_t tid;
                       int *array;
                       int size;
                       double max;  
                       int max_index;
                       int start_index;
};


double function(double element)
{
    return (sqrt(element) * sin(element));
}

int find_max_serial(int *array, int size, double *maxval)
{
     int ctr, index;
     *maxval = function(array[0]);
     index = 0;
     for(ctr=1; ctr < size; ++ctr){
           double x = function(array[ctr]);
           if(x > *maxval){
                *maxval = x;
                index = ctr;
           }
     }          
     return index;
}

void* find_max(void *arg)
{
     struct thread_param *param = (struct thread_param *) arg;
     (void) param;

     int ctr;
     param->max = function(param->array[0]);
     param->max_index = 0;
     for(ctr=1; ctr < param->size; ++ctr){
           double x = function(param->array[ctr]);
           if(x > param->max){
                param->max = x;
                param->max_index = ctr;
           }
     }          

     return NULL;
}

int main(int argc, char **argv)
{
  struct thread_param *params;
  struct timeval start, end;
  int *a, num_elements, ctr, num_threads, th_pos = 0;
  double max = 0.0;
  int max_index = 0;

  if(argc !=3)
           USAGE_EXIT("not enough parameters");

  num_elements = atoi(argv[1]);
  if(num_elements <=0)
          USAGE_EXIT("invalid num elements");
  
  num_threads = atoi(argv[2]);
  if(num_threads <=0 || num_threads > MAX_THREADS){
          USAGE_EXIT("invalid num of threads");
  }


  /* Parameters seems to be alright. Lets start our business*/

  a = malloc(num_elements * sizeof(int));
  if(!a){
          USAGE_EXIT("invalid num elements, not enough memory");
  }

  srand(SEED);  
  for(ctr=0; ctr<num_elements; ++ctr)
        a[ctr] = random();


  /*Allocate thread specific parameters*/
  params = malloc(num_threads * sizeof(struct thread_param));
  bzero(params, num_threads * sizeof(struct thread_param));

  gettimeofday(&start, NULL);

  /*Partion data and create threads*/      
  for(ctr=0; ctr < num_threads; ++ctr){
        struct thread_param *param = params + ctr;
        param->size = num_elements/num_threads;
        if(ctr == 0){
            param->size += num_elements % num_threads;    //residue elements
        }
        param->array = a + th_pos;
        param->start_index = th_pos;
        th_pos += param->size;
        if(pthread_create(&param->tid, NULL, find_max, param) != 0){
              perror("pthread_create");
              exit(-1);
        }
 
  }
  
  assert(th_pos == num_elements);
  th_pos = 0;
  /*Wait for threads to finish their execution*/      
  for(ctr=0; ctr < num_threads; ++ctr){
        struct thread_param *param = params + ctr;
        pthread_join(param->tid, NULL);
        if(ctr == 0) {
          max = param->max;
          max_index = param->max_index;
          continue;
        }
        if (max < param->max) {
          max = param->max;
          max_index = param->max_index + param->start_index;
        }

  }
     
  printf("PARALLEL\n");
  printf("Max = %.2f for %d at index=%d\n", max, a[max_index], max_index);
  gettimeofday(&end, NULL);
  printf("Time taken = %ld microsecs\n", TDIFF(start, end));

  gettimeofday(&start, NULL);

  max = 0.0;
  ctr = find_max_serial(a, num_elements, &max);
  gettimeofday(&end, NULL);
  printf("SERIAL\n");
  printf("Max = %.2f for %d at index=%d\n", max, a[ctr], ctr);
  
  printf("Time taken = %ld microsecs\n", TDIFF(start, end));

  free(a);
  free(params);
}
