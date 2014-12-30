#!/bin/bash

# Based on iot-lab_M3 debug.sh script.
# Author: Jonas Remmert j.remmert@phytec.de

elffile=`echo $1 | sed 's/.hex/.elf/'`

red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m'

echo The file: $elffile will be flashed to the board: $BOARD .
echo Press y to proceed, v to proceed with verify or any other key to abort.
read x

retval=$(arm-none-eabi-readelf -x .fcfield $elffile  | awk '/0x00000400/ {print $2 $3 $4 $5}')
unlocked_fcfield="fffffffffffffffffffffffffeffffff"

if [ "$retval" != "$unlocked_fcfield" ]; then
echo "Hexdump, .fcfield of $elffile"
retval=$(arm-none-eabi-readelf -x .fcfield $elffile)
echo $retval
echo -e "Fcfield not fitting to MKW01Z128, danger of bricking the device during flash...${red} [ABORT]${NC}"
exit 0
fi
echo -e "Flash configuration tested...${green} [OK]${NC}"


# flash without verify
#if [ $x = "y" ] || [ $x = "v" ];# -o $x -eq v ];
#then
echo -e "${red}Flashing device, do not disconnect or reset the target until this script exits!!!${NC}"
openocd -f "$RIOTBASE/boards/pba-d-01-kw01/dist/mkw01z128.cfg" \
	-c "init" \
	-c "reset halt" \
        -c "flash write_image erase $elffile" \
	-c "reset run" \
	-c "exit"
#fi

# verify
#if [ "$x" = "v" ];
#then
#echo -e "${red}Flashing device, do not disconnect or reset the target until this script exits!!!${NC}"
#openocd -f "$RIOTBASE/boards/pba-d-01-kw01/dist/mkw01z128.cfg" \
#        -c "init" \
#        -c "reset halt" \
#        -c "verify_image $elffile" \
#        -c "reset run" \
#        -c "exit"
#fi
echo -e "${green}done...${NC}"
