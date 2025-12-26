#!/bin/sh

# This script resets the only the CM4 using JLink called
# with a pre-defined reset sequence.

# @author Dylan Laduranty <dylan.laduranty@mesotic.com>

BINDIR=$1

# create JLink command file for resetting the board
echo "Sleep 5" > $BINDIR/reset.seg
echo "w4 0x3039000C 0x000000A8" >> $BINDIR/reset.seg
echo "Sleep 500" >> $BINDIR/reset.seg
echo "w4 0x3039000C 0x000000AA" >> $BINDIR/reset.seg
echo "exit" >> $BINDIR/reset.seg
cp $BINDIR/reset.seg /home/dylan/reset.seg

# reset the board
JLinkExe -device MIMX8MQ6_M4 -if JTAG -speed 4000 -autoconnect 1 < $BINDIR/reset.seg
