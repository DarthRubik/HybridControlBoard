
C Code for the Hybrid Control Board
=======

## Getting started

In order to build the C Code for this project you will obviously need a copy of this repository.  This project uses cmake as a build program, so it should be rather straight forward.

Before you try to build, you should note that you are likely going to have to install the cpputest library which is as simple as running:
`apt-get install cpputest`.  Next you have to tell cmake where it is, so you have to define two environment variables: `CPPUTEST_BIN` and `CPPUTEST_INCLUDE` (TODO: change this).  On my install they are defined as:

~~~~
export CPPUTEST_BIN=/usr/lib/i386-linux-gnu
export CPPUTEST_INCLUDE=/usr/include
~~~~

They could be different on your PC

Next in theory all that is left to do is create a build directory and run cmake:

~~~~
mkdir build
cd build
cmake path/to/c_src
~~~~

Cmake will then create all of the build files and you should just have to run make.  Note: after each build the unit tests will automatically run.
