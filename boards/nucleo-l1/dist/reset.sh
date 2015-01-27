#!/bin/bash

echo "##"
echo "## Resetting $1"
echo "##"

# Needs OpenOCD version 0.9.0 built after Jan. 26th 2015

openocd -f "board/st_nucleo_l1.cfg" \
    -c "init" \
    -c "reset run"\
    -c "shutdown"
