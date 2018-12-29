#ifndef __COMMAND_H__
#define __COMMAND_H__


#define STHREAD_STACK_SIZE (1 << 20)
#define STHREAD_STACK_MASK 0xfffffffffff00000UL
#define MAX_DEVNAME 256
#define DEFAULT_DEV "/dev/sthreads"
#define DEVNAME "sthreads"

enum command_type_t{
                           REG_KEY,
                           UNREG_KEY,
                           REG_ERR_FN,
                           UNREG_ERR_FN,
                           MAX_CMDS
};

struct command_t{
                      long command;
                      unsigned long param;         
};
  
#endif
