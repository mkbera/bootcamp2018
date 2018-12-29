#ifndef __COMMAND_H__
#define __COMMAND_H__


#define DEVNAME "sthreads"

enum command_type_t{
                           REG_KEY,
                           UNREG_KEY,
                           REG_ERR_FN,
                           UNREG_ERR_FN,
                           MAX_CMDS
};

struct process_info{
                        pid_t pid;
                        int is_valid_key;
                        int is_valid_callback;
                        unsigned long key;
                        unsigned long error_callback;
};

struct command_t{
                      long command;
                      unsigned long param;         
};
  
#endif
