#!/bin/bash

openocd -f "board/atmel_samr21_xplained_pro.cfg" \
    -c "init" \
    -c "targets" \
    -c "reset halt" \
    -c "reset init" \
    -c "flash write_image erase $1" \
    -c "verify_image $1" \
    -c "reset run" \
    -c "shutdown"
