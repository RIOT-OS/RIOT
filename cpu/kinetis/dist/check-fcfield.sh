#!/bin/sh
#
# Flash configuration field check script for Freescale Kinetis MCUs.
#
# This script is supposed to be called from RIOTs
# unified OpenOCD script (dist/tools/openocd/openocd.sh).
#
# Syntax: check-fcfield.sh $FLASHFILE
#
# @author       Jonas Remmert <j.remmert@phytec.de>
# @author       Johann Fischer <j.fischer@phytec.de>
# @author       Joakim Nohlgård <joakim.nohlgard@eistec.se>
# @author       Francisco Molina <francisco.molina@inria.fr>

# Elf or Hex file to flash
FLASHFILE="$1"

# FCFIELD goes from 0x400-0x40f
FCFIELD_START='0x400'
FCFIELD_END='0x410'
FCFIELD_AWK_REGEX='^ 0400 '

# The following fcfield places the flash in a unsecure state which is the default
# FCF code for Freescale Kinetis MCUs, see /cpu/kinetis/fcfield.c.
EXPECTED_FCFIELD="^fffffffffffffffffffffffffe..ffff$"

get_fc_field()
{
    if [ ${1##*.} = elf ]; then
        arm-none-eabi-objdump -j.fcfield -s "${1}"
    elif [ ${1##*.} = hex ]; then
        arm-none-eabi-objdump --start-address=${FCFIELD_START} --stop-address=${FCFIELD_END} ${1} -s
    else
        echo "Unkown file extension: ${1##*.}"
        exit 1
    fi
}

filter_fc_field()
{
    get_fc_field "${1}" | awk -F' ' "/${2}/ { print \$2 \$3 \$4 \$5; }"
}

if [ $# -ne 1 ]; then
    echo "Usage: $0 FLASHFILE"
    echo "Checks the flash configuration field protection bits to avoid flashing a locked image to a Kinetis MCU (protection against accidental bricking)."
    exit 2
fi

if ! filter_fc_field ${FLASHFILE} ${FCFIELD_AWK_REGEX} | grep -q "${EXPECTED_FCFIELD}"; then
    echo "Danger of bricking the device during flash!"
    echo "Flash configuration field of ${FLASHFILE}:"
    get_fc_field ${FLASHFILE}
    echo "Abort flash procedure!"
    exit 1
fi
echo "${FLASHFILE} is not locked."
exit 0
