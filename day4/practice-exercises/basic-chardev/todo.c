#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#include "module/command.h"

void print_pinfo(struct process_info *pinfo)
{
   printf("pid = %d is_valid_key = %d key=%lx is_valid_callback = %d error_callback=%lx\n", 
           pinfo->pid, pinfo->is_valid_key, pinfo->key, pinfo->is_valid_callback, pinfo->error_callback);
}
int main()
{
   int fd;
   struct command_t cmd;
   struct process_info info;
   printf("My PID is %d ADDR[print_pinfo] = %p\n", getpid(), &print_pinfo);
   fd  = open("/dev/sthreads",O_RDWR);
   if(fd < 0){
       perror("open");
       exit(-1);
   }
  
  if(read(fd, &info, sizeof(info)) < 0){
      perror("read");
      exit(-1);
  }
  print_pinfo(&info);

  cmd.command = REG_KEY;
  cmd.param = 0x199999;
  if(write(fd, &cmd, sizeof(cmd)) < 0){
      perror("write");
      exit(-1);
  } 
  
  cmd.command = REG_ERR_FN;
  cmd.param = (unsigned long)(&print_pinfo);
  if(write(fd, &cmd, sizeof(cmd)) < 0){
      perror("write");
      exit(-1);
  } 
  
  if(read(fd, &info, sizeof(info)) < 0){
      perror("read");
      exit(-1);
  }
  print_pinfo(&info);
  
  close(fd); 
  return 0;
}
