CC=gcc

ifeq ($(DEBUG),1)
    CFLAGS:=-Wall -Werror -g 
else
    CFLAGS:=-Wall -Werror -O3 
endif

LDFLAGS=-lm -lpthread

all:serialmax parallelmax blockhash

serialmax:find_max_serial.c 
	$(CC) $(CFLAGS) find_max_serial.c $(LDFLAGS) -o serialmax
parallelmax:find_max_parallel.c 
	$(CC) $(CFLAGS) find_max_parallel.c $(LDFLAGS) -o parallelmax
blockhash:block_hash.c
	$(CC) $(CFLAGS) block_hash.c $(LDFLAGS) -o blockhash

clean:
	rm -f *.o serialmax parallelmax blockhash

.PHONY: clean	
