#!/bin/sh

openocd -f "board/stm32f4discovery.cfg" \
    -c "tcl_port 6333" \
    -c "telnet_port 4444" \
    -c "init" \
    -c "targets" \
    -c "reset halt" \
    -l /dev/null &

# save pid to terminate afterwards
OCD_PID=$?

# needed for openocd to set up
sleep 2

arm-none-eabi-gdb -tui -command=$1 $2

kill ${OCD_PID}
