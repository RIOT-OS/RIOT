#!/bin/sh
#
# Anti brick check script for Freescale Kinetis MCUs.
#
# This script is supposed to be called from RIOTs
# unified OpenOCD script (dist/tools/openocd/openocd.sh).
#
# @author       Jonas Remmert <j.remmert@phytec.de>
# @author       Johann Fischer <j.fischer@phytec.de>
# @author       Francisco Acosta <francisco.acosta@inria.fr>

ELFFILE=$1

# Check if MCUBoot is enabled. If so, increase the memory addresses by the
# BOOTLAODER_OFFSET + VTOR_OFFSET + MCUBOOT_HDR
if [ -n "${MCUBOOT}" ]; then
	let "FCFIELD = $BOOTLOADER_OFFSET+$MCUBOOT_HDR+$VTOR_OFFSET"
	FCFIELD_ADDR=`printf "0x%08x\n" $FCFIELD`
else
	FCFIELD_ADDR=`printf "0x%08x\n" $VTOR_OFFSET`
fi

RETVAL=$(arm-none-eabi-readelf -x .fcfield $ELFFILE  | awk "/${FCFIELD_ADDR}/ {print \$2 \$3 \$4 \$5}")
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
