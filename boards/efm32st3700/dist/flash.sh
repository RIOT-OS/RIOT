#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Flashing: $2 to processor: $1 address: $3"

arm-none-eabi-objcopy -O binary $2.elf $2.bin

sed s+BINARY+$2.bin+g $DIR/flashscript.txt > flash.jlink

JLinkExe -device $1 -speed 4000 -CommanderScript flash.jlink