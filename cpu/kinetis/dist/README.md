K60 tools
=========

This directory contains tools for working with K60 CPUs.

Watchdog disable
----------------

wdog-disable.bin is a location-independent watchdog disable function with a
breakpoint instruction at the end. Useful for disabling the watchdog directly
from OpenOCD.

Usage:

    openocd -c 'reset halt' \
      -c 'load_image wdog-disable.bin 0x20000000 bin' \
      -c 'resume 0x20000000' # watchdog is disabled and core halted

The file is saved generated in the repository to remove the need to have a
compiler when flashing. It is only 34 bytes binary.
