#!/bin/sh

# check if GDB_PORT is already defined
if [ -z ${GDB_PORT} ]; then
    # set to gdb standard port
    GDB_PORT=3333
fi
echo "Listening for GDB connection on port ${GDB_PORT}"

openocd -f "board/stm32f0discovery.cfg" \
    -c "tcl_port 6333" \
    -c "telnet_port 4444" \
    -c "gdb_port ${GDB_PORT}" \
    -c "init" \
    -c "targets" \
    -c "reset halt" \
    -l /dev/null &

# save pid to terminate afterwards
OCD_PID=$?

# needed for openocd to set up
sleep 2

arm-none-eabi-gdb -tui -command="${RIOTBOARD}/${BOARD}/dist/gdb.cfg" ${ELFFILE}

kill ${OCD_PID}
