#!/bin/sh

# This script downloads downloads and extracts the msp430 support files,
# then removes currently unused headers and linker scripts.

set -e

URL="https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/9_2_0_0/export/msp430-gcc-support-files-1.210.zip"
rm -Rf msp430-gcc-support-files

_CPUS="$(git -C ../../.. grep -o '^CPU_MODEL.=.*430.*$' | cut -d' ' -f 3 | sort -u)"
wget $URL
unzip $(basename $URL)

(
    cd msp430-gcc-support-files/include
    rm $(ls | grep -v -E '(msp430\.h|in430\.h|legacy\.h|iomacros\.h|devices.csv)' | \
            grep -v -F "${_CPUS}" )
)
