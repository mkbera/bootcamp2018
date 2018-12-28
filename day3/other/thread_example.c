#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<assert.h>

struct account_t{
                long ac_balance;
                long num_transactions;
};
                 
                  
void *do_deposit(void *arg){
   long i;
   struct account_t *account = (struct account_t *) arg;
   for(i=0; i < account->num_transactions; ++i){
       account->ac_balance += 100 + random() % 100;
   }
   pthread_exit(NULL);
}

int main(int argc, char **argv) 
{
   int residue;
   unsigned num_threads, ctr;
   pthread_t *threads;
   struct account_t account;
   if(argc != 3){
         printf("Usage: %s [#of threads] [#of transactions]\n", argv[0]);
         exit(-1);
   }
   
   num_threads = atoi(argv[1]);
   if(num_threads > 32){
          printf("Max threads should be even and can be max 32\n");
          exit(-1);
   }
   
   account.num_transactions = atoll(argv[2]);
   assert(account.num_transactions > num_threads);

   account.num_transactions /= num_threads;  
   residue = account.num_transactions % num_threads;
   account.ac_balance = 0;

   threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
   
   for(ctr=0; ctr < num_threads; ++ctr){
       pthread_create (threads + ctr, NULL, do_deposit, &account);
   }

   for(ctr=0; ctr<residue; ++ctr){
       account.ac_balance += 100 + random() % 100;
   }
   for(ctr=0; ctr < num_threads; ++ctr)
           pthread_join(threads[ctr], NULL);
   printf("remaining balance=%ld\n",account.ac_balance);
}
