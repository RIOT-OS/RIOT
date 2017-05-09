#!/bin/sh

if [ -L "$0" ]; then
    FILE=$(readlink "$0")
else
    FILE="$0"
fi

BIN_FOLDER=$(dirname "${FILE}")

echo "##"
echo "## Resetting $BOARD"
echo "##"
openocd -f "${BIN_FOLDER}/openocd.cfg" \
    -c "init" \
    -c "reset run" \
    -c "shutdown"
