import os

rmmod = 'sudo rmmod sthreads'
os.system(rmmod)

insmod = 'sudo insmod solution/sthreads.ko'
os.system(insmod)


