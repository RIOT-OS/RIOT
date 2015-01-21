#!/bin/sh

echo "##"
echo "## Starting debug server"
echo "##"

openocd -f board/st_nucleo_f334r8.cfg \
    -c "init" \
    -c "targets" \
    -c "reset halt"
