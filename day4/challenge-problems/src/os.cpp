#include "os.h"
using namespace os;

// Constructor for std file stream
osinstream_t::osinstream_t(const char *filename)
  : std::ifstream(filename),
    valid(false),
    count(0),
    fd(-1), 
    fptr(NULL),
    fpos(0),
    filesize(0)
{
}
// Constructor for os file stream
osinstream_t::osinstream_t(const char *filename, bool os)
  : std::ifstream(filename),
    valid(false),
    count(NULL),
    fd(-1), 
    fptr(NULL),
    fpos(0),
    filesize(0)
{
   struct stat sbuf;
   
   //Open the file 
   fd =::open(filename, O_RDONLY);
   if(fd < 0){
         perror("open");
         return;
   }
    
   // Find the size of the file
   if(fstat(fd, &sbuf) < 0){
         perror("stat");
         ::close(fd);
         return;
   }
   
  filesize = sbuf.st_size;
  
  // MMAP the file into memory and prepopulate them
  fptr = mmap(NULL, filesize, PROT_READ, MAP_SHARED|MAP_POPULATE, fd, 0);
  if(fptr == MAP_FAILED){
       perror("mmap");
       ::close(fd);
       return;
  }
  assert(pthread_mutex_init(&lock, NULL) == 0);
  // Now the stream is valid to be read from
  valid = true;

  // Initialize usage count to 1
  count = new std::atomic <int>();
  *count = 1;;
}

//Copy constructor. 
// Creates a reference to the same memory where the file is mapped.
// Can be used to maintain different streams for different threads
// This scheme does not protect *safe* access from different objects
// Safe (non-overlapping) access is ensured only for a single object
  
osinstream_t::osinstream_t(osinstream_t &obj)
{
  valid = obj.valid;
  if(valid){
      fd = obj.fd;
      fptr = obj.fptr;
      fpos = obj.fpos;
      filesize = obj.filesize;
      assert(pthread_mutex_init(&lock, NULL) == 0);
      count = obj.count;
      (*count)++;
  }     
}

long osinstream_t::read(char *ptr, long size)
{
   if(!valid){
         std::ifstream::read(ptr, size);
         return size;
   }
   assert(valid);
   assert(size > 0);
   assert(ptr);
   
   //Serialize access from multiple threads through the same object
   pthread_mutex_lock(&lock);
   if(fpos + size > filesize){
       std::cout << "Error: reading beyond the file size" << std::endl;
       pthread_mutex_unlock(&lock);
       return -1;
   }
  long cpos = fpos;
  fpos += size;
  pthread_mutex_unlock(&lock);
  memcpy(ptr, (void *)((unsigned long)fptr + cpos), size);
  return size; 
}

// Destructor for the mmap based file stream
osinstream_t::~osinstream_t()
{
   if(valid){ // Destroy Only if it is valid
         assert(*count >= 1);
         (*count)--;
         if(*count == 0){
            munmap(fptr, filesize);
            ::close(fd);
        }
        assert(pthread_mutex_destroy(&lock) == 0);
   }
}

chdev_t *chdev_t::chdev_instance = NULL; //static initialization

chdev_t* chdev_t::get_chdev()
{
    if(chdev_instance)
           return chdev_instance;
    chdev_instance = new chdev_t();
    assert(chdev_instance);
    strcpy(chdev_instance->devname, DEFAULT_DEV);
    if(chdev_instance->dev_open() < 0){
         delete chdev_instance;
         chdev_instance = NULL;
    }
    return chdev_instance;         
}

chdev_t::~chdev_t()
{
   if(isopen && getpid() == _owner){
       assert(dev >= 0);
       close(dev);
    }
}

int chdev_t::register_key(unsigned long key)
{
   struct command_t cmd = {REG_KEY, key};
   return dev_command(&cmd);
}

int chdev_t::unregister_key(unsigned long key)
{
   struct command_t cmd = {UNREG_KEY, key};
   return dev_command(&cmd);
}

int chdev_t::register_error_function(void)
{
   struct command_t cmd = {REG_ERR_FN, (unsigned long) &::handle_bad_thread};
   return dev_command(&cmd);
}
int chdev_t::unregister_error_function(void)
{
   struct command_t cmd = {UNREG_ERR_FN, 0};
   return dev_command(&cmd);
}
//Thread manager implementation
thmgr_t::thmgr_t(int num_threads = MAX_THREADS)
:num_threads(num_threads),
 chdev(NULL)
{ 
   assert(num_threads > 0 && num_threads <= MAX_THREADS);
   threads = (struct thread_t *)malloc(num_threads * sizeof(struct thread_t));
   assert(threads);
   secure = false;   
   for(int i=0; i<num_threads; ++i){
      threads[i].tid = i;
      threads[i].used = false;
      threads[i].running = false;
      threads[i].stack = NULL;
      threads[i].thfunc = NULL;
      threads[i].arg = NULL;    
   }
} 


thmgr_t::thmgr_t(chdev_t *chdev, int num_threads = MAX_THREADS)
:num_threads(num_threads),
 chdev(chdev)
{ 
   assert(num_threads > 0 && num_threads <= MAX_THREADS);
   threads = (struct thread_t *)malloc(num_threads * sizeof(struct thread_t));
   assert(threads);
   secure = true;;   
   assert(chdev);
   for(int i=0; i<num_threads; ++i){
      threads[i].tid = i;
      threads[i].used = false;
      threads[i].running = false;
      threads[i].stack = NULL;
      threads[i].thfunc = NULL;
      threads[i].arg = NULL;    
   }
} 
struct thread_t * thmgr_t::find_free_thread()
{
   int i;
   for(i = 0; i < num_threads && threads[i].used; ++i);
   if(i < num_threads)
       return (threads + i);
   else 
       return NULL;    
   
}
//Create a thread. Do not launch it yet
long thmgr_t::create(void *(*tfunc)(void *), void *tharg)
{
   struct thread_t *th = find_free_thread();
   if(!th){
       std::cout << "All threads busy" << std::endl;
       return -1;
   }
  assert(!th->used);
  th->used = true;
  th->thfunc = tfunc;
  th->arg = tharg; 
  assert(pthread_attr_init(&th->attr) == 0);     
  if(secure){
       th->stack = mmap(NULL, STHREAD_STACK_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED|MAP_POPULATE|MAP_STACK, 0, 0);
       assert(th->stack != MAP_FAILED);
       assert(pthread_attr_setstack(&th->attr, th->stack, STHREAD_STACK_SIZE) == 0);
  }
  return th->tid;
}

long thmgr_t::launch(long tid)
{
   assert(tid >= 0 && tid < num_threads);
   struct thread_t *th = threads + tid;
   assert(!th->running && th->thfunc);
   if(secure){
      assert(th->stack);
      unsigned long key = random();
      assert(chdev->register_key(key) >= 0);
      unsigned long s = (unsigned long) th->stack + STHREAD_STACK_SIZE;
      s = s - (1 << 13);
      //printf("Key address = %lx key=%lx\n", s, key);
      *((unsigned long *) s) = key;
      assert(pthread_create(&th->_tid, &th->attr, th->thfunc, th->arg)==0);
   }else{
      assert(pthread_create(&th->_tid, &th->attr, th->thfunc, th->arg)==0);
   }
   th->running = true;
   return 0;  
}

long thmgr_t::wait(long tid)
{
   assert(tid >= 0 && tid < num_threads);
   struct thread_t *th = threads + tid;
   assert(th->running);
   pthread_join(th->_tid, NULL);
   if(secure){
       munmap(th->stack, STHREAD_STACK_SIZE);
   }
   th->running = false;
   th->thfunc = NULL;
   return 0;
}

long thmgr_t::wait()
{
   for(int i=0; i<num_threads; ++i){
       struct thread_t *th = threads + i;
       if(th->running){
           assert(th->used);
           wait(i);
           th->used = false;
       }
   }
   return 0;
}

thmgr_t::~thmgr_t()
{
   wait();  // Just in case
   free(threads);
   if(secure && chdev)
     delete chdev;
      
}
//Global handler for misbehaving threads
void handle_bad_thread(void)
{
    //pthread_detach(pthread_self());
    std::cout << "The thread with ID ["<< syscall(SYS_gettid) << "] has not registered the key. Can be a mallicious user!!" << std::endl;
    std::cout << "Killing the thread ..." << std::endl;
    syscall(SYS_exit, 0);
}

//Attacker thread
void *thread_func(void *arg)
{
    for(int i=0; i<5; ++i) {
        std::cout << "Hoohaa... Attack through pthread-gate successful!" << std::endl;
        sleep(1);
    }
   pthread_exit(NULL);
}

