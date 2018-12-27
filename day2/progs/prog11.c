#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_THREADS 2

uint32_t numLockAcqs;

// FIXME: What is wrong in this instrumentation function?
// fname is the function that is being instrumented
void __lock_acq(char *fname) {
  printf("Lock acquired: %s\n", fname);
  numLockAcqs++;
}

void __lock_rel(char *fname) {
  printf("Lock released: %s\n", fname);
  // numLockAcqs++;
}


uint32_t counter;
pthread_mutex_t count_mutex;

struct thr_args {
  char *name;
  uint16_t id;
};

void increment_count() {
  pthread_mutex_lock(&count_mutex);
  counter += 1;
  pthread_mutex_unlock(&count_mutex);
}

uint32_t get_count() {
  uint32_t c;
  pthread_mutex_lock(&count_mutex);
  c = counter;
  pthread_mutex_unlock(&count_mutex);
  return (c);
}

void *thrBody(void *arguments) {
  struct thr_args *tmp = arguments;
  printf("Thread %d has started\n", tmp->id);
  pthread_mutex_lock(&count_mutex);
  counter += 1;
  pthread_mutex_unlock(&count_mutex);
  printf("Thread %d has ended\n", tmp->id);
  return NULL;
}

int main() {
  int i = 0;
  int error;
  pthread_t tid[NUM_THREADS];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  struct thr_args args[NUM_THREADS] = {0};

  while (i < NUM_THREADS) {
    args[i].name = "Thread";
    args[i].id = i;
    error = pthread_create(&tid[i], &attr, &thrBody, args + i);
    if (error != 0) {
      printf("\nThread can't be created : [%s]", strerror(error));
    }
    i++;
  }

  sleep(5);

  return 0;
}
