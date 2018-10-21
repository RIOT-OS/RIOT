# ARM Semihosting STDIO

This module provides stdio over JTAG/SWD using ARM's semihosting support.

## Usage

Add `USEMODULE += stdio_semihosting` to your application's Makefile

Tell OpenOCD to enable semihosting with `arm semihosting enable`. One way to
do this is to add the following to your application's Makefile

    export OPENOCD_DBG_START_CMD = -c 'reset halt' -c 'arm semihosting enable'

## Limitations

* Reads and writes block all threads on the processor. This is a limitation of
  the Semihosting technology
* It is somewhat slow
* The image cannot be used without a debugger attached

## Advantages

Provides IO for any ARM device with a JTAG/SWD connection without any extra
hardware or pins