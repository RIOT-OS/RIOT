#!/bin/bash

echo "##"
echo "## Resetting $1"
echo "##"

openocd -f "${RIOTBOARD}/${BOARD}/dist/openocd.cfg" \
    -c "init" \
    -c "reset run"\
    -c "shutdown"
