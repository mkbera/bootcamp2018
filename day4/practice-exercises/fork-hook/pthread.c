#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<assert.h>
#include<pthread.h>
#include<sys/mman.h>

void *thfunc(void *arg)
{
    printf("I am in thread\n");
    pthread_exit(NULL);
}

int main()
{
   void *th_stack;
   pthread_attr_t th_attr;
   pthread_t tid;
   printf("mypid = %d\n", getpid());
   
    assert(pthread_attr_init(&th_attr) == 0);     
    th_stack = mmap(NULL, 1 << 14, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED|MAP_POPULATE|MAP_STACK, 0, 0);
   assert(th_stack != MAP_FAILED);
   printf("The threads stack spans from [%p - %p]\n", th_stack, th_stack + (1 << 14));
   assert(pthread_attr_setstack(&th_attr, th_stack, 1 << 14) == 0);
   assert(pthread_create(&tid, &th_attr, thfunc, NULL) == 0);
   
   // assert(pthread_create(&tid, NULL, thfunc, NULL) == 0);
   assert(pthread_join(tid, NULL) == 0);
   munmap(th_stack, 1 << 14);
   return 0;
}
