#!/usr/bin/env bash

# The setsid command is needed so that Ctrl+C in GDB doesn't kill avarice
: ${SETSID:=setsid}

if avr-gdb -v > /dev/null; then
    GDB=avr-gdb
elif gdb-multiarch -v > /dev/null; then
    GDB=gdb-multiarch
else
    echo "Couldn't find multiarch GDB or AVR GDB. Check \$PATH."
    exit 1
fi

sleep 2
${SETSID} -w avarice $1 &
sleep 3 && $GDB -ex "target remote localhost:$3" $4

# avarice exits with 1 if the connection is released, therefore we always exit with 0
exit 0
