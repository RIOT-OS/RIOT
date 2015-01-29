#!/bin/bash

echo "##"
echo "## Resetting $1"
echo "##"

openocd -f "board/stm32f3discovery.cfg" \
    -c "init" \
    -c "tcl_port 0" \
    -c "telnet_port 0" \
    -c "gdb_port 0" \
    -c "reset run"\
    -c "shutdown"
