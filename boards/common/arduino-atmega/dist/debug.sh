#!/usr/bin/env bash
sleep 2
setsid -w avarice $1 &
#sleep 2 && $2/avr-gdb-wrapper -ex "target remote localhost:$3" $4
sleep 3 && avr-gdb -ex "target remote localhost:$3" $4

# avarice exits with 1 if the connection is released, therefore we always exit with 0
exit 0
