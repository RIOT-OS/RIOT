#!/bin/bash

RIOTBASE=$1
APPLICATION=$2
BOARD=$3
APPLICATION=$4

echo "device nrf51822" > $RIOTBASE/boards/$BOARD/dist/burn.seg
echo "speed 1000" >> $RIOTBASE/boards/$BOARD/dist/burn.seg
echo "w4 4001e504 1" >> $RIOTBASE/boards/$BOARD/dist/burn.seg
echo "loadbin $RIOTBASE/examples/$APPLICATION/bin/$BOARD/$APPLICATION.bin 0" >> $RIOTBASE/boards/$BOARD/dist/burn.seg
echo "r" >> $RIOTBASE/boards/$BOARD/dist/burn.seg
echo "g" >> $RIOTBASE/boards/$BOARD/dist/burn.seg
echo "exit" >> $RIOTBASE/boards/$BOARD/dist/burn.seg
echo "" >> $RIOTBASE/boards/$BOARD/dist/burn.seg
JLinkExe < $RIOTBASE/boards/$BOARD/dist/burn.seg
