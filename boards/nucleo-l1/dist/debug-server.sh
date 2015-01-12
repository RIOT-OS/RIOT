#!/bin/sh

echo "##"
echo "## Starting debug server"
echo "##"

openocd -f "${RIOTBOARD}/${BOARD}/dist/openocd.cfg" \
    -c "init" \
    -c "targets" \
    -c "reset halt"
