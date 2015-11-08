#!/bin/sh

# Start in-circuit debugging on this board: this script starts up the GDB
# client and connects to a GDB server.
#
# Start the GDB server first using the 'make debugserver' target

# @author Attilio Dona'

CFGDIR=$1
ELFFILE=$2

cd $CFGDIR

# run GDB
arm-none-eabi-gdb -tui -command=gdbinit $ELFFILE
