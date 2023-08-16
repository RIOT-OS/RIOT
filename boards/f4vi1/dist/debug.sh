#!/bin/sh

echo "Debugging $1"
arm-none-eabi-gdb -tui -command=$1 $2
