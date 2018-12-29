#ifndef __OS_H_DEFINED__
#define __OS_H_DEFINED__

#include <iostream>
#include <fstream>
#include<atomic>

extern "C" {
    #include<stdlib.h>
    #include<unistd.h>
    #include<fcntl.h>
    #include<pthread.h>
    #include<sys/mman.h>
    #include<sys/types.h>
    #include<pthread.h>
    #include<sys/stat.h>
    #include<sys/wait.h>
    #include <assert.h>
    #include<string.h>
    #include<sys/syscall.h>
    #include<limits.h>
    #include "module/command.h"
};

extern void handle_bad_thread(void);
extern void *thread_func(void *arg);
namespace os
{
  
   class osinstream_t : public std::ifstream
   {
       private:
                 bool valid;     // Is the stream valid
                 std::atomic <int> *count; // usage count
                 int fd;         // File descriptor
                 void *fptr;     // mmaped VA
                 long fpos;      // current fileposition
                 long filesize;   // Size of the file
                 pthread_mutex_t lock;  //Needed to make read MT safe
       public:
                osinstream_t(const char *filename);
                osinstream_t(const char *filename, bool os);
                osinstream_t(osinstream_t &obj);
                long read (char *ptr, long size);    
                ~osinstream_t();

                inline bool isvalid() const
                {
                    return valid;
                }
                // Returns 0 on success
                // -1 on error
                inline long seek(long pos)
                {    
                     if(pos < 0 || pos >= filesize)
                        return -1;
                     pthread_mutex_lock(&lock);
                     fpos = pos;
                     pthread_mutex_unlock(&lock);
                }
               inline uint8_t *getraw(void)
               {
                     return (uint8_t *)fptr;
               } 
               inline long getsize(void)
               {
                    return filesize;
               }
  };
   
  //Singleton class for chardev
  class chdev_t
  { 
      private: 

                 static chdev_t *chdev_instance;
                 char devname[MAX_DEVNAME];
                 int dev;
                 pid_t _owner;
                 bool isopen;
                 
                 // private member functions
                 // No default constructors should be public
                  
                  chdev_t():_owner(getpid()),isopen(false){};  
	          chdev_t(chdev_t const&){};             
	          chdev_t& operator=(chdev_t const &) = delete;   //c++11  

                 inline int dev_command(struct command_t *command)
                 {
                       assert(isopen);
                       return write(dev, command, sizeof(struct command_t));
                 }

                 inline int dev_open()
                 {
                     assert(!isopen);
                     dev = open(devname, O_RDWR);
                     if(dev < 0) {
                        perror("open");
                        return -1;
                     } 
                     isopen = true;
                     return 0;
                 }
                 inline int dev_close()
                 {
                     assert(open);
                     close(dev);
                     isopen = false;
                     return 0;
                 }
                 
      public:
                 ~chdev_t();
                 static chdev_t *get_chdev();
                 int register_key(unsigned long key);
                 int unregister_key(unsigned long key);
                 int register_error_function(void); 
                 int unregister_error_function(void);                      
        }; 
  
  #define MAX_THREADS 32
  struct thread_t{
                     long tid; // Thread manager maintained thread ID  
                     pthread_t _tid;    // System thread ID
                     bool used;
                     bool running;
                     void *(*thfunc)(void *);
                     void *stack;
                     void *arg;
                     pthread_attr_t attr;
                  };
                     
  class thmgr_t{
                  protected:
                            struct thread_t *threads;
                            bool secure;
                            int num_threads;
                            chdev_t *chdev;
	                    thmgr_t(thmgr_t const&){};             
	                    thmgr_t& operator=(thmgr_t const &) = delete;   //c++11  
                            struct thread_t* find_free_thread(void); 
                  public:
                              thmgr_t(int num_threads);
                              thmgr_t(chdev_t *chdev, int num_threads);
                              ~thmgr_t();
                              long create(void *(*tfunc)(void *), void *tharg); //Returns the tid
                              long launch(long tid); //Returns 0 if successfully launched
                              long wait(long tid); // Wait for a given thread ID
                              long wait(void); // Wait for all sthreads
                                         
  };
}

#endif //__OS_H_DEFINED_
