Mixing C++ and C
================
This project demonstrates and tests to mix a user `c++` main application with plain `c` compiled RIOT-OS.
It is based on the [Using-CPP-on-RIOT](https://github.com/RIOT-OS/RIOT/wiki/Using-CPP-on-RIOT) guide.

Makefile Options
----------------
* __CPPMIX__ Turns on the mixedmode for user `.cpp` with RIOT `.c` code when set to 1
* __CPPMIXCC__ The compiler call to build the user `.cpp` files, e.g. `g++`
* __CPPMIXEXTRALIBS__ The full path(s) to addional libs that should be linked to RIOT (_note, the scribed libs will be linked before the BASELIBS_)
* __CPPMIXCFLAGS__ Additional compiler flags to build user `.cpp` file, e.g. `-std=c++11`
