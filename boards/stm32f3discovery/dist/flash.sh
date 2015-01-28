#!/bin/bash

echo "##"
echo "## Flashing $1"
echo "##"

openocd -f "board/stm32f3discovery.cfg" \
    -c "init" \
    -c "tcl_port 0" \
    -c "telnet_port 0" \
    -c "gdb_port 0" \
    -c "targets" \
    -c "reset halt" \
    -c "program $1 0x08000000 verify" \
    -c "reset run"\
    -c "shutdown"
