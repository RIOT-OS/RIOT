#!/bin/sh

openocd -f "${RIOTBOARD}/${BOARD}/dist/atmel_samr21_xpro.cfg" \
    -c "init" \
    -c "targets" \
    -c "reset halt" \
    -c "reset init" \
    -c "flash write_image erase $1" \
    -c "verify_image $1" \
    -c "reset run"\
    -c "shutdown"
