#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<pthread.h>

#define MAX_THREADS 64
#define BLOCK_SIZE 256

static char *dataptr;
static unsigned long *optr;
pthread_mutex_t lock;


unsigned long calculate_and_store_hash(char *ptr, char *endptr)
{
    double hv;
    unsigned hashval = 0;
    int count = BLOCK_SIZE/sizeof(unsigned long);
    while(count--){
         if((endptr - ptr) < sizeof(unsigned long))
                break;
         hv = (double) hashval / 71111.123;
         hashval = hv * 71111.0;
         hashval += *((unsigned*)ptr);
         ptr += sizeof(unsigned);
    }
   return hashval;
}

void *hashit(void *arg)
/*Argument is the end pointer*/
{
   char *cptr;
   unsigned long *chash;
   char *endptr = (char *)arg;   // END pointer

   while(1){
        pthread_mutex_lock(&lock);
        if(dataptr >= endptr){
          pthread_mutex_unlock(&lock);      
          break;
        }
        cptr = dataptr;
        dataptr += BLOCK_SIZE;
        chash = optr;
        optr++; 
        pthread_mutex_unlock(&lock);
        /*Perform the real calculation*/
        *chash = calculate_and_store_hash(cptr, endptr); 
   
  }
  pthread_exit(NULL); 
}

int main(int argc, char **argv)
{
     int fd, ctr, num_threads;
     unsigned long size, hash_count;
     char *buff;
     unsigned long *hashes;
     pthread_t *threads;

     pthread_mutex_init(&lock, NULL);

     struct stat sbuf;
     if(argc != 3){
            printf("Usage: %s <fileneme> <num_threads>\n", argv[0]);
            exit(-1);         
      }
       
     num_threads = atoi(argv[2]);
     if(num_threads <=0 || num_threads > MAX_THREADS){
            printf("Usage: %s <fileneme> <num_threads>\n", argv[0]);
            exit(-1);         
     }

     fd = open(argv[1], O_RDONLY);
     if(fd < 0){
           printf("Can not open file\n");
           exit(-1);
     } 
    
     if(fstat(fd, &sbuf) < 0){  
         perror("stat");
         exit(-1);
     }
     
    size = sbuf.st_size;;
  
    // MMAP the file into memory and prepopulate them
    buff = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if(buff == MAP_FAILED){
       perror("mmap");
       close(fd);
       exit(-1);
    }
    // At this point buff points to the start of the file 
    hash_count = size / BLOCK_SIZE;
    if(size % BLOCK_SIZE)
          ++hash_count;

    hashes = malloc(sizeof(unsigned long) * hash_count);
    if(!hashes){
           perror("mem");
           exit(-1);
    }   
     
    dataptr = buff;
    optr = hashes; 
    threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));
    if(!threads){
           perror("mem");
           exit(-1);
    }   

    for(ctr=0; ctr < num_threads; ++ctr){
        if(pthread_create(threads + ctr, NULL, hashit, buff + size) != 0){
              perror("pthread_create");
              exit(-1);
        }
     }
     
     for(ctr=0; ctr < num_threads; ++ctr)
            pthread_join(threads[ctr], NULL);
 
    
     for(ctr=0; ctr < hash_count; ++ctr)
           printf("block# %d hash %lx\n", ctr, hashes[ctr]);  
     
     free((void *)hashes);
     free((void *)threads);
     munmap(buff, size);
     close(fd);

     pthread_mutex_destroy(&lock);
}

