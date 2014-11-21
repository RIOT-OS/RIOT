#!/bin/bash

openocd -f "${RIOTBOARD}/${BOARD}/dist/${BOARD}_jtag.cfg" \
    -f "target/stm32f1x.cfg" \
    -c "tcl_port 6333" \
    -c "telnet_port 4444" \
    -c "init" \
    -c "targets" \
    -c "reset halt" \
    -l /dev/null &

# save pid to terminate afterwards
OCD_PID=$?

# needed for openocd to set up
sleep 5

arm-none-eabi-gdb -tui --command=${1} ${2}

kill ${OCD_PID}
