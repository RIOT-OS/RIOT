#!/bin/sh

if [ -L "$0" ]; then
    FILE=$(readlink -e "$0")
else
    FILE="$0"
fi

BIN_FOLDER=$(dirname "${FILE}")

echo "##"
echo "## Starting debug server"
echo "##"
openocd -f "${BIN_FOLDER}/openocd.cfg" \
    -c "init" \
    -c "targets" \
    -c "reset halt" \
