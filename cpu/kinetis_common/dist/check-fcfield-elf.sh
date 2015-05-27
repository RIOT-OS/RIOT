#!/bin/sh
#
# Anti brick check script for Freescale Kinetis MCUs.
#
# This script is supposed to be called from RIOTs
# unified OpenOCD script (dist/tools/openocd/openocd.sh).
#
# @author       Jonas Remmert <j.remmert@phytec.de>
# @author       Johann Fischer <j.fischer@phytec.de>

ELFFILE=$1

RETVAL=$(arm-none-eabi-readelf -x .fcfield $ELFFILE  | awk '/0x00000400/ {print $2 $3 $4 $5}')
UNLOCKED_FCFIELD="fffffffffffffffffffffffffeffffff"

if [ "$RETVAL" != "$UNLOCKED_FCFIELD" ]; then
    echo "Danger of bricking the device during flash!"
    echo "Flash configuration field of $ELFFILE:"
    arm-none-eabi-readelf -x .fcfield $ELFFILE
    echo "Abort flash procedure!"
    exit 1
fi
echo "$ELFFILE is fine."
exit 0
