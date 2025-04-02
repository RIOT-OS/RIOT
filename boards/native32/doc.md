@defgroup    boards_native32 Native32 Board
@ingroup     boards
@brief       Support for running RIOT natively on a 32-bit host platform

# Overview

The RIOT native boards use system calls and signals to emulate hardware at the API level.
That means that you can compile the complete RIOT software stack for your *NIX system
and run it as a process.

`BOARD=native32` as one of two possible native variants will compile to and execute
a 32-bit executable. For 64-bit systems, it is recommended to use @ref boards_native64
instead. `BOARD=native` will automatically select the right variant depending on the
host platform. For more information on the native boards please refer to
@ref boards_common_native.

# Required packages

You will need cross-compiling support for 32-bit executables if you are on a 64-bit
host platform. On Debian/Ubuntu you can install the required libraries with

```
sudo apt install gcc-multilib g++-multilib lib32asan5
```

`g++-multilib` is only needed for C++ support. `libasan5` is needed for unittest
execution of 32 bit binaries.
