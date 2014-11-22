#!/bin/sh

openocd -f "board/stm32f4discovery.cfg" \
    -c 'tcl_port 0' \
    -c 'gdb_port 0' \
    -c 'telnet_port 0' \
    -c "init" \
    -c "reset run" \
    -c "shutdown"
