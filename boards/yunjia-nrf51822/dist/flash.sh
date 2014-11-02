#!/bin/sh

if [ ! -f "$1" ]; then
    echo "Binary file $1 does not exist"
    exit 1
fi

if [ -L "$0" ]; then
    FILE=$(readlink "$0")
else
    FILE="$0"
fi

BIN_FOLDER=$(dirname "${FILE}")

echo "##"
echo "## Flashing $1"
echo "##"
openocd -f "${BIN_FOLDER}/openocd.cfg" \
    -c "init" \
    -c "targets" \
    -c "flash banks" \
    -c "reset halt" \
    -c "flash write_image erase $1 0" \
    -c "verify_image $1" \
    -c "reset run"\
    -c "shutdown"
