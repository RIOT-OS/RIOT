#!/bin/sh

echo "##"
echo "## Debugging ${ELFFILE}"
echo "##"

# if GDB_PORT does not exist or holds empty string
if [ -z ${GDB_PORT} ]; then
    # set to gdb standard port
    GDB_PORT=3333
else
    echo "Listening for GDB connection on port ${GDB_PORT}"
fi

openocd -f "board/stm32f3discovery.cfg" \
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

arm-none-eabi-gdb -tui -command=${RIOTBOARD}/${BOARD}/dist/gdb.conf ${ELFFILE}

kill ${OCD_PID}
