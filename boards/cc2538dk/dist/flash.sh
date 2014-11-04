#!/bin/sh

# This flash script dynamically generates a file with a set of commands which
# have to be handed to the flashing script of SEGGER (JLinkExe >4.84).
# After that, JLinkExe will be executed with that set of commands to flash the
# latest .bin file to the board.

# @author Hauke Petersen <hauke.petersen@fu-berlin.de>

BINDIR=$1
HEXFILE=$2
FLASHADDR=200000

# setup JLink command file
echo "speed 1000" >> $BINDIR/burn.seg
echo "loadbin $HEXFILE $FLASHADDR" >> $BINDIR/burn.seg
echo "r" >> $BINDIR/burn.seg
echo "g" >> $BINDIR/burn.seg
echo "exit" >> $BINDIR/burn.seg

# flash new binary to the board
JLinkExe -device CC2538SF53 < $BINDIR/burn.seg
echo ""
