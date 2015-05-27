#!/bin/sh
#
# Flash configuration field check script for Freescale Kinetis MCUs.
#
# This script is supposed to be called from RIOTs
# unified OpenOCD script (dist/tools/openocd/openocd.sh).
#
# Syntax: check-fcfield-hex.sh $HEXFILE
#
# @author       Jonas Remmert <j.remmert@phytec.de>
# @author       Johann Fischer <j.fischer@phytec.de>
# @author       Joakim Gebart <joakim.gebart@eistec.se>

if [ $# -ne 1 ]; then
    echo "Usage: $0 HEXFILE"
    echo "Checks the flash configuration field protection bits to avoid flashing a locked image to a Kinetis MCU (protection against accidental bricking)."
    exit 2
fi

HEXFILE="$1"
FCFIELD_START='0x400'
FCFIELD_END='0x410'
FCFIELD_AWK_REGEX='^ 0400 '

ACTUAL_FCFIELD=$(arm-none-eabi-objdump --start-address=${FCFIELD_START} --stop-address=${FCFIELD_END} ${HEXFILE} -s | awk -F' ' "/${FCFIELD_AWK_REGEX}/ { print \$2 \$3 \$4 \$5; }")
EXPECTED_FCFIELD="fffffffffffffffffffffffffeffffff"

if [ "${ACTUAL_FCFIELD}" != "${EXPECTED_FCFIELD}" ]; then
    echo "Danger of bricking the device during flash!"
    echo "Flash configuration field of ${HEXFILE}:"
    arm-none-eabi-objdump --start-address=${FCFIELD_START} --stop-address=${FCFIELD_END} ${HEXFILE} -s
    echo "Abort flash procedure!"
    exit 1
fi
echo "${HEXFILE} is not locked."
exit 0
