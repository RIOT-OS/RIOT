#!/bin/bash

echo "##"
echo "## Resetting $1"
echo "##"

openocd -f board/st_nucleo_f334r8.cfg \
    -c "init" \
    -c "reset run"\
    -c "shutdown"
