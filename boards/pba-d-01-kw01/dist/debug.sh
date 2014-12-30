#!/bin/bash

# Based on iot-lab_M3 debug.sh script.
# Author: Jonas Remmert j.remmert@phytec.de

red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m'

openocd -f "$RIOTBASE/boards/pba-d-01-kw01/dist/mkw01z128.cfg" \
    -c "tcl_port 6333" \
    -c "telnet_port 4444" \
    -c "init" \
    -c "targets" \
    -c "reset halt" \
    -l /dev/null &

# save pid to terminate afterwards
OCD_PID=$?

# needed for openocd to set up
sleep 1

retval=$(arm-none-eabi-readelf -x .fcfield $2  | awk '/0x00000400/ {print $2 $3 $4 $5}')
unlocked_fcfield="fffffffffffffffffffffffffeffffff"

if [ "$retval" == "$unlocked_fcfield" ]; then
echo -e "Flash configuration tested...${green} [OK]${NC}"
arm-none-eabi-gdb -tui --command=${1} ${2}
#cgdb -d arm-none-eabi-gdb --command=${1} ${2}

else
echo "Hexdump, .fcfield of $2"
retval=$(arm-none-eabi-readelf -x .fcfield $2)
echo $retval
echo -e "Fcfield not fitting to MKW01Z128, danger of bricking the device during flash...${red} [ABORT]${NC}"
fi
kill ${OCD_PID}
