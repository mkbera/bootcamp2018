CC = gcc
CFLAGS = -g

all: mat mat-opt

mat: mergesort.o test_mat.o matrix.o testing.o
	$(CC) $^ -o $@

mat-opt: mergesort.o test_mat_opt.o matrix.o testing.o
	$(CC) $^ -o $@

clean:
	rm -f *.o mat mat-opt
