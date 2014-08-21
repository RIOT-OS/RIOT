#!/bin/bash

if [ -L "$0" ]; then
    FILE=$(readlink "$0")
else
    FILE="$0"
fi

BIN_FOLDER=$(dirname "${FILE}")

openocd -f "${BIN_FOLDER}/${BOARD}_jtag.cfg" \
    -f "target/stm32f1x.cfg" \
    -c "init" \
    -c "reset run" \
    -c "shutdown"
