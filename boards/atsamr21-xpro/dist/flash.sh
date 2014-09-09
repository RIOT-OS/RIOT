#!/bin/env sh

openocd -f "${RIOTBOARD}/${BOARD}/dist/atmel_samr21_xplained_pro.cfg" \
    -c "init" \
    -c "targets" \
    -c "shutdown"
