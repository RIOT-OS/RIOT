@defgroup    boards_native64 Native64 Board
@ingroup     boards
@brief       Support for running RIOT natively on a 64-bit host platform

# Overview

The RIOT native boards use system calls and signals to emulate hardware at the API level.
That means that you can compile the complete RIOT software stack for your *NIX system
and run it as a process.

`BOARD=native64` as one of two possible native variants will compile to and execute
a 64-bit executable. You may use @ref boards_native32 instead for 32-bit executables.
`BOARD=native` will automatically select the right variant depending on the host
platform. For more information on the native boards please refer to @ref boards_common_native.
