#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<assert.h>
#include<pthread.h>
void *thfunc(void *arg)
{
    printf("I am in thread\n");
    pthread_exit(NULL);
}

int main()
{
   pthread_t tid;
   printf("mypid = %d\n", getpid());
   assert(pthread_create(&tid, NULL, thfunc, NULL) == 0);
   assert(pthread_join(tid, NULL) == 0);
   return 0;
}
