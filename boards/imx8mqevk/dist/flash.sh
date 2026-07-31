#!/bin/sh

# This flash script dynamically generates a file with a set of commands which
# have to be handed to the flashing script of SEGGER (JLinkExe >=V6.46).
# After that, JLinkExe will be executed with that set of commands to flash the
# latest .bin file to the board.

# @author Dylan Laduranty <dylan.laduranty@mesotic.com>

BINDIR=$1
HEXFILE=$2
FLASHADDR=0x1FFE0000

# setup JLink command file
echo "speed 4000" > $BINDIR/burn.seg
echo "loadfile $HEXFILE $FLASHADDR" >> $BINDIR/burn.seg
echo "w4 0x3039000C 0x000000A8" >> $BINDIR/burn.seg
echo "Sleep 50" >> $BINDIR/burn.seg
echo "w4 0x3039000C 0x000000AA" >> $BINDIR/burn.seg
echo "exit" >> $BINDIR/burn.seg
# flash new binary to the board
JLinkExe -device MIMX8MQ6_M4 -if JTAG -speed 4000 -autoconnect 1 < $BINDIR/burn.seg
