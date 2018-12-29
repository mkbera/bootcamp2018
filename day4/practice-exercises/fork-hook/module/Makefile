obj-m = sthreads.o

KVERSION = $(shell uname -r)
ccflags-y += -DDBG

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean

