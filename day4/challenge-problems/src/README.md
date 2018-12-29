# The code base

The code base is a muti-threaded implementation done in day 3.
There is a thread manager class `thmgr_t` and a chardev wrapper `chdev_t` which 
enable the user space part of the secure thread design. Our objective is to disallow 
all process creation (using fork/vfork) and thread creation except using the secure
`thmgr_t` API.

The module directory contains all the necessary functionalities except for the correct 
`fork_hook` logic. You can build the module using $make$

The solution module object is in the solution directory.

# Compilation and sanity check

Run make.

$./readindexed blocks_4096_64.idx   // Should complete successfully

# Experiencing what-to-do

Uncomment the lines in readindexed.cpp (line #217-234) and run $make$.
Insert the module built from the source (in module directory)

Execute the following
 
$./readindexed blocks_4096_64.idx --secure

Unload the module and insert the solution module (inside solution directory)

Re-execute the above program. Notice the difference.

#My promise:  The solution module is compiled from a code base which differs only in function `fork_hook`

#Your job:  

To write appropriate code to make it equivalent to the solution code.
