import os

make = 'make'
os.system(make)

rmmod = 'sudo rmmod sthreads'
os.system(rmmod)

insmod = 'sudo insmod sthreads.ko'
os.system(insmod)

del_cmd = 'sudo dmesg -c'
os.system(del_cmd)



