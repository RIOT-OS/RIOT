#!/bin/sh

if [ ! -f "$2" ]; then
    echo "ELF-file $2 does not exist"
    exit 1
fi

echo "##"
echo "## Debugging $2"
echo "##"
arm-none-eabi-gdb -tui -command="$1" $2
