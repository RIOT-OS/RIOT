#!/bin/sh

openocd -f "board/stm32f4discovery.cfg" \
    -c 'tcl_port 0' \
    -c 'gdb_port 0' \
    -c 'telnet_port 0' \
    -c "init" \
    -c "targets" \
    -c "reset" \
    -c "halt" \
    -c "flash protect 0 0 11 off" \
    -c "flash write_image erase $1 0 ihex" \
    -c "verify_image $1" \
    -c "reset run"\
    -c "shutdown"
