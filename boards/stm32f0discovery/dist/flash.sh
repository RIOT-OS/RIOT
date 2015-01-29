#!/bin/sh

openocd -f "board/stm32f0discovery.cfg" \
    -c 'tcl_port 0' \
    -c 'gdb_port 0' \
    -c 'telnet_port 0' \
    -c "init" \
    -c "targets" \
    -c "reset halt" \
    -c "flash write_image erase ${HEXFILE} 0" \
    -c "verify_image ${HEXFILE}" \
    -c "reset run"\
    -c "shutdown"
