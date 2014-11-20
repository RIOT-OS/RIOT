#!/bin/sh

if [ ! -f "$1" ]; then
    echo "ELF-file $1 does not exist"
    exit 1
fi

if [ -L "$0" ]; then
    FILE=$(readlink "$0")
else
    FILE="$0"
fi

BIN_FOLDER=$(dirname "${FILE}")

echo "##"
echo "## Debugging $1"
echo "##"
arm-none-eabi-gdb -tui -command="${BIN_FOLDER}/gdb.cfg" $1
