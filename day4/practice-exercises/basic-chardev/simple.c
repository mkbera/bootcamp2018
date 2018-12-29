#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int main()
{
   char buf[256]; 
   int fd;
   char *msg = "Hello, I am from user";
   printf("My PID is %d\n", getpid());
   fd  = open("/dev/sthreads",O_RDWR);
   if(fd < 0){
       perror("open");
       exit(-1);
   }
  
  if(write(fd, msg, strlen(msg)) < 0){
      perror("write");
      exit(-1);
  } 
  
  if(read(fd, buf, 256) < 0){
      perror("read");
      exit(-1);
  }
  
  printf("Message from kernel: %s\n", buf);
  close(fd); 
  return 0;
}
