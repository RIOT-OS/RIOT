## Supported CPUs

This code was tested on the MSP-EXP430FR5969 board featuring the MSP430FR5969 microcontroller. Other chips should work as well.

## Memory Problems

If you experience problems with your program, for example printf is not workig, it might be due to memory limitations.  The newlib version shipped with the TI uses too much memory too be useful.

Possible fixes:

* reduce stack sizes, look for THREAD_STACKSIZE_DEFAULT
* recompile gcc/newlib
  * https://sourceware.org/newlib/README
  * https://github.com/pabigot/bsp430/blob/master/maintainer/msp430-elf-build
* provide custom libc functions
