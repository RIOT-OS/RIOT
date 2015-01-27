#!/bin/bash

echo "##"
echo "## Resetting $1"
echo "##"

openocd -f "board/stm32f3discovery.cfg" \
    -c "init" \
    -c "reset run"\
    -c "shutdown"
