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
ACTUAL_FCFIELD=$(arm-none-eabi-objdump -j.fcfield -s "${ELFFILE}"  | awk '/^ 0400 / {print $2 $3 $4 $5}')
# Allow any FOPT flags configuration (".." in the pattern)
EXPECTED_FCFIELD="^fffffffffffffffffffffffffe..ffff$"

if ! printf '%s' "${ACTUAL_FCFIELD}" | grep -q "${EXPECTED_FCFIELD}" ; then
    echo "Danger of bricking the device during flash!"
    echo "Flash configuration field of ${ELFFILE}:"
    arm-none-eabi-objdump -j.fcfield -s "${ELFFILE}"
    echo "Abort flash procedure!"
    exit 1
fi
echo "${ELFFILE} is fine."
exit 0
