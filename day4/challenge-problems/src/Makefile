all:    readindexed 

#DEBUG:=1

ifeq ($(DEBUG),1)
    CPPFLAGS:=-Wall -Werror -g -std=c++11 
else
    CPPFLAGS:=-Wall -Werror -O3 -std=c++11
endif
LDFLAGS:=-L/usr/local/lib -lssl -lcrypto -lpthread

%.o: %.cpp %.h
	g++ -c ${CPPFLAGS} $< -o $@



readindexed: readindexed.o transaction.o crypto.o blockchain.o os.o
	g++ ${CPPFLAGS} -o readindexed readindexed.o transaction.o crypto.o blockchain.o os.o ${LDFLAGS}


clean:
	rm -f a.out *.o genchain readchain readindexed rewritechain cryptotest 

.PHONY: clean
