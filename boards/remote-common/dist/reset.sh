#!/bin/sh

# This script resets a CC2538SF53 target using JLink called
# with a pre-defined reset sequence.

# @author Hauke Petersen <hauke.petersen@fu-berlin.de>

BINDIR=$1

# create JLink command file for resetting the board
echo "r" >> $BINDIR/reset.seg
echo "g" >> $BINDIR/reset.seg
echo "exit" >> $BINDIR/reset.seg

# reset the board
JLinkExe -device CC2538SF53 < $BINDIR/reset.seg
echo ""
