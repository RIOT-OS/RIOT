#!/bin/sh

# Start in-circuit debugging on this board: this script starts up the GDB
# client and connects to a GDB server.
#
# Start the GDB server first using the 'make debugserver' target

BINDIR=$1
ELFFILE=$2

# write GDB config file
echo "target extended-remote 127.0.0.1:2331" > $BINDIR/gdb.cfg

# run GDB
arm-none-eabi-gdb -tui -command=$BINDIR/gdb.cfg $ELFFILE
