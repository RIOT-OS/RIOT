#!/bin/bash

echo "##"
echo "## Flashing $1"
echo "##"

openocd -f "${RIOTBOARD}/${BOARD}/dist/openocd.cfg" \
    -c "init" \
    -c "targets" \
    -c "reset halt" \
    -c "program $1 0x8000000 verify" \
    -c "reset run"\
    -c "shutdown"
