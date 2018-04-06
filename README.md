# Overview

This is Richard Williams' comments concerning the Edgar coding challenge for the Data Science Fellowship program.
The original readme file is contained in README_orig.md

# Development Environment

The primary development environment for this coding challenge was a Windows 10 machine using Visual Studio.  As a result you will see some Visual Studio project files, some Windows style line endings, and perhaps some files encoded in UTF-16.

However, the code is portable and has been verified to run on my old obsolete 32-bit Linux machine.  The code has been verified to run under Windows, mingw, and Linux Mint.  When examining the code you will find multiple instances where the code could be simplified by using a different library call.  I did not use some common library functions in order to maintain maximum portability.  For example, I did not use the function "strptime" for this reason.

# Unit Tests

The code passes the provided test case and an additional small test case of my creation.  I created a number of additional test cases, however my test case files are very large, as large as 2GB.  Github does not allow files larger than 100MB.  As such, my test cases have not been uploaded to Github.  

# Assumptions

* I am assuming the test environment is a 32-bit Linux environment.  While the code will run fine on a 64-bit machine, if I built my code for an 64-bit environment it would not run in a 32-bit environment.
* It is assumed the number of characters used to hid the last three digits of the IP address is three characters or less.
* I am using GNU g++ to build my code under Linux.
* I am not aware of any special requirements to run my code other than a standard Linux system.

# Performance

Most of my testing has been done with downloaded Edgar log files.  They are each around 2GB in size uncompressed.  My program processes these files at around 100MB/s. on my mediocre Windows machine.  Much slower on my obsolete 32-bit Linux machine.  This speed is close to the sequential read performance of rotational hard disks.  With some more code optimization, the performance bottleneck will be rotational hard disk speed.  Faster performance would require a solid state hard disk.

More than half of the CPU cycles in my code are spent in "fgets".  For performance to be improved file I/O needs to be made faster.  It is possible to at least cut the number of CPU cycles in half by performing I/O in larger blocks than what "fgets" is using.  Spending time optimizing non-file I/O and non-string processing code will not result in any signification speed improvements.

There are some obvious code optimizations that can be made that I did not do because the speed improvement would be trivial.  It is more important that the code is simple to understand.

The vector entry size is kept small in order to improve resize performance.

# Vectors versus linked lists

One area of contention is going to be that I stored the current sessions data in a vector rather than using a linked list.  Using vectors has an implied resizing through memcpys.  Such resizing is not needed if using linked lists.

I have profiled the code and the amount of time used for resizing on actual Edgar source files is trivial.  Replacing the vectors with linked lists will not result in better performance using actual Edgar data.

The standard "school" performance comparison between the two would be that a linked lists are faster because fewer operations are performed.  In actual real measured performance, performance of "small" vectors is better than linked lists even when resizing is taken into account.  When resizing is performed, modern processors are capable of performing block moves in L1 cache of 64-bit chunks of 64-bit aligned data very fast.

Linked lists will result in better performance if the number of concurrent users of the Edgar database is "very large".  However, when I examined the Edgar database there wasn't a "very large" number of concurrent users.  Vectors would need to be changed to linked list if the number of concurrent users is larger than the numbers I have seen.

# Possible optimizations

* The code can be made faster if the reasonable assumption is made that a session can not last more than one day.
* The size of entries in the vector "sessions" can be cut in half and thereby halving the time required to resize the vector.
* File I/O, in particular "fgets", can optimized to run about twice as fast, or perhaps faster.  "fgets" is reading the data into memory in too small chunks.

# Larger files

* I am not aware of anything special required to process files larger than 2 GB.  There should not be a problem processing 1 TB files.  Currently on my mediocre computer it should be possible to process a 1 TB file in about 16 minutes.  With some improvements in how I/O is performed the execution time can be cut in half and perhaps more.
* At some point it is possible that 32-bit operations are insufficient for very large files and 64-bit file operations are required.  Some testing would be required to find if there are any such limitations.  I will need to set up a better test environment than the very slow 32-bit Linux environment that I am using.
* The only real issue relates to the storing of the intermediate values in a vector rather than as a linked list and this issue was discussed above.
