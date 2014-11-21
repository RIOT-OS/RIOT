#!/bin/bash

openocd -f "${RIOTBOARD}/${BOARD}/dist/${BOARD}_jtag.cfg" \
    -f "target/stm32f1x.cfg" \
    -c "init" \
    -c "targets" \
    -c "reset halt" \
    -c "reset init" \
    -c "flash write_image erase $1" \
    -c "verify_image $1" \
    -c "reset run"\
    -c "shutdown"
