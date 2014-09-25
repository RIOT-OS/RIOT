#!/bin/sh

# This script resets a nrf51822 target using JLink called
# with a pre-defined reset sequence.

# @author Hauke Petersen <hauke.petersen@fu-berlin.de>

BINDIR=$1

# create JLink command file for resetting the board
echo "device nrf51822" > $BINDIR/reset.seg
echo "r" >> $BINDIR/reset.seg
echo "g" >> $BINDIR/reset.seg
echo "exit" >> $BINDIR/reset.seg
echo " " >> $BINDIR/reset.seg

# reset the board
JLinkExe < $BINDIR/reset.seg
