#!/usr/bin/env bash

# The setsid command is needed so that Ctrl+C in GDB doesn't kill avarice
: ${SETSID:=setsid}

sleep 2
${SETSID} -w avarice $1 &
sleep 3 && avr-gdb -ex "target remote localhost:$3" $4

# avarice exits with 1 if the connection is released, therefore we always exit with 0
exit 0
