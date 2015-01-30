#!/bin/bash

echo "##"
echo "## Flashing ${HEXFILE}"
echo "##"

openocd -f "board/stm32f3discovery.cfg" \
    -c "tcl_port 0" \
    -c "telnet_port 0" \
    -c "gdb_port 0" \
    -c "init" \
    -c "targets" \
    -c "reset halt" \
    -c "program ${HEXFILE} 0x08000000 verify" \
    -c "reset run"\
    -c "shutdown"
