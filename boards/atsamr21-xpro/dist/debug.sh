#!/bin/bash

openocd -f "${RIOTBOARD}/${BOARD}/dist/atmel_samr21_xpro.cfg" \
    -c "tcl_port 6333" \
    -c "telnet_port 4444" \
    -c "init" \
    -c "targets" \
    -c "reset halt"\
