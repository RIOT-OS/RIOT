#!/bin/sh

if [ ! -f "$2" ]; then
    echo "ELF-file $2 does not exist"
    exit 1
fi

# if GDB_PORT does not exist or holds empty string
if [ -z ${GDB_PORT} ]; then
    GDB_PORT=0
fi

echo "##"
echo "## Debugging $2"
echo "##"

openocd -f "board/stm32f3discovery.cfg" \
    -c "tcl_port 6333" \
    -c "telnet_port 4444" \
    -c "gdb_port ${GDB_PORT}"
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
