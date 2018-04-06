# Overview

This is Richard Williams' comments concerning the Edgar coding challenge for the Data Science Fellowship program.
The origional readme file is contained in README_orig.md

# Development Environment

The primary development environment for this coding challenge was a Windows 10 machine using Visual Studio.  As a result you will see you will see Visual Studio project files, some Windows style file endings, and perhaps some files encoded in UTF-16.

However, the code is portable and has been verified to run on my old obsolete 32-bit Linux machine.  The code has been verified to run under Windows, mingw, and Linux Mint.  While examining the code you will find multiple places where the code could be simplified by using a different library call.  I did not use many common library functions in order to maintain maximum portability.  An example function is "strptime".

# Unit Tests

The code passes the provided test case and an additional small test case of my creation.  I did create a number of additional test cases, however my test cases are very large.  Some of them are 2GB in size.  Github does not allow files greater than 100MB.  As such, my test cases have not been uploaded to Github.

# Assumptions

* I am assuming the test environment is a 32-bit Linux environment.  While the code will run fine on a 64-bit machine, if I assumed the target environment was 64-bit while it is actually 32-bit my code would not work.
* It is assumed the characters used to hid the last three digits of the IP address is three characters or less.

# Performance

Most of my testing has been done with the downloaded Edgar log files.  They are each around 2GB in size (uncompressed).  My program processes these files at around 100MB/s. on my mediocre performance Windows machine.  (Much slower on my obsolete 32-bit Linux machine.)  This is close to the sequential performance of rotational hard disks.  With some more code optimization, the performance bottleneck will be rotational hard disk speed.  Faster performance would require a solid state hard disk.

There are some obvious code optimizations that can be made, but I have not done them because the speed improvements would be trivial.  It is more important that the code look simple.

The vector entry size is kept small in order to improve performance.

# Possible optimizations

* The code can be made faster if the reasonable assumption is made that a session can not last more than one day.
* The size of entries in the vector "sessions" can be cut in halved and thereby halving the time required to resize the array.
* File I/O in particular "fgets" can optimized to run about twice as fast, or perhaps faster.  The code is reading the data into memory in too small chunks.

# Larger files

* Need to use 64-bit file operations
* There should not be a problem processing 1 TB files.  Currently on my mediocre machine this can be done in about 16 minutes.  With some improvements in how I/O is performed the execution time can be cut in half and perhaps much less.
