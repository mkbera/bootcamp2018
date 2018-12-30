CC = gcc
CFLAGS = -g

all: prog_a prog_b SOA AOS

prog_a: prog_a.o
	$(CC) $^ -o $@

prog_b: prog_b.o
	$(CC) $^ -o $@

SOA: SOA.o
	$(CC) $^ -o $@

AOS: AOS.o
	$(CC) $^ -o $@

clean:
	rm -f *.o prog_a prog_b SOA AOS
