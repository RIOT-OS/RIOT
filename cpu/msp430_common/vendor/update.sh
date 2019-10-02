#!/bin/sh

# This script downloads downloads and extracts the msp430 support files,
# then removes currently unused headers and linker scripts.

set -e

URL="https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/latest/exports/msp430-gcc-support-files-1.208.zip"
rm -Rf msp430-gcc-support-files

_CPUS="$(git -C ../../.. grep -o '^CPU_MODEL.=.*430.*$' | cut -d' ' -f 3 | sort -u)"
wget $URL
unzip $(basename $URL)

(
    cd msp430-gcc-support-files/include
    rm $(ls | grep -v -E '(msp430\.h|in430\.h|legacy\.h|iomacros\.h|devices.csv)' | \
            grep -v -F "${_CPUS}" )
)
