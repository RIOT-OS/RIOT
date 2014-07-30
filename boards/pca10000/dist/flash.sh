#!/bin/bash

# This flash script dynamically generates a file with a set of commands which
# have to be handed to the flashing script of SEGGER (JLinkExe).
# After that, JLinkExe will be executed with that set of commands to flash the
# latest .bin file to the board.

# @author Timo Ziegler <timo.ziegler@fu-berlin.de>

BINDIR=$1
RIOTBASE=$2
APPLICATION=$3
BOARD=$4

if [[ $APPLICATION == test_* ]]
then
	TYPE=tests
else
	TYPE=examples
fi

echo "log /dev/null" > $BINDIR/burn.seg
echo "device nrf51822" >> $BINDIR/burn.seg
echo "speed 1000" >> $BINDIR/burn.seg
echo "w4 4001e504 1" >> $BINDIR/burn.seg
echo "loadbin $RIOTBASE/$TYPE/$APPLICATION/bin/$BOARD/$APPLICATION.bin 0" >> $BINDIR/burn.seg
echo "r" >> $BINDIR/burn.seg
echo "g" >> $BINDIR/burn.seg
echo "exit" >> $BINDIR/burn.seg
echo "" >> $BINDIR/burn.seg
JLinkExe < $BINDIR/burn.seg
if [ -f JLink.log ]
then
  rm JLink.log
fi
if [ -f ~/JLink.log ]
then
  rm ~/JLink.log
fi
