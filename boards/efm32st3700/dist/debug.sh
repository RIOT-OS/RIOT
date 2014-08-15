#!/bin/sh

echo "Debugging $1 application: $2"

arm-none-eabi-gdb -tui -command=$3 $2.elf
