#!/bin/bash

echo "##"
echo "## Resetting ${BOARD}"
echo "##"

openocd -f "board/stm32f3discovery.cfg" \
    -c "tcl_port 0" \
    -c "telnet_port 0" \
    -c "gdb_port 0" \
    -c "init" \
    -c "reset run"\
    -c "shutdown"
