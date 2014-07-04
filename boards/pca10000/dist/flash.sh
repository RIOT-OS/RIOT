#!/bin/bash

BINDIR=$1
RIOTBASE=$2
APPLICATION=$3
BOARD=$4

echo "log /dev/null" > $BINDIR/burn.seg
echo "device nrf51822" >> $BINDIR/burn.seg
echo "speed 1000" >> $BINDIR/burn.seg
echo "w4 4001e504 1" >> $BINDIR/burn.seg
echo "loadbin $RIOTBASE/examples/$APPLICATION/bin/$BOARD/$APPLICATION.bin 0" >> $BINDIR/burn.seg
echo "r" >> $BINDIR/burn.seg
echo "g" >> $BINDIR/burn.seg
echo "exit" >> $BINDIR/burn.seg
echo "" >> $BINDIR/burn.seg
JLinkExe < $BINDIR/burn.seg
rm JLink.log
