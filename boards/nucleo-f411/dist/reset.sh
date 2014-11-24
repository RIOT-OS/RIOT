#!/bin/sh

openocd -f "board/st_nucleo_f411re.cfg" \
    -c "init" \
    -c "reset run" \
    -c "shutdown"
