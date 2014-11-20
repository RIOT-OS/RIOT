#!/bin/bash

openocd -f "${RIOTBOARD}/${BOARD}/dist/${BOARD}_jtag.cfg" \
    -f "target/stm32f1x.cfg" \
    -c "init" \
    -c "reset run" \
    -c "shutdown"
