#!/bin/sh

echo "##"
echo "## Starting debug server"
echo "##"

# Needs OpenOCD version 0.9.0 built after Jan. 26th 2015

openocd -f "board/st_nucleo_l1.cfg" \
    -c "init" \
    -c "targets" \
    -c "reset halt"
