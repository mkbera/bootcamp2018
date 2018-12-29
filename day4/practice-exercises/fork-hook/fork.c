#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include "module/command.h"

int main()
{
   pid_t pid;
   pid = fork();
   if(pid < 0){
         perror("");
         exit(-1);
   }
   
   if(!pid){
         printf("I am child pid=%d\n", getpid());
   }else{
           printf("I am parent pid = %d\n", getpid());
   }

/*   while(1)
     sleep(1);*/
   return 0;
}
