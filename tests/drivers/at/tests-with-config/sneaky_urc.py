#!/usr/bin/env python3

# This script simulates a modem sending a "sneaky URC", a URC that happens exactly
# after the command is received, but before the first line of answer is sent. This
# is possible behavior at least on the U-Blox LTE modules.
#
# Running this test requires the board to be connected twice
#  - one connection simulating the modem (e.g.: /dev/ttyUSB0), connected to the
#    serial device this script is listening to
#  - one to access the RIOT CLI (e.g.: debugger, USB or a second serial connection)
#
# How to get it running:
#  1. Adapt the `EOL_IN`, `EOL_OUT`, `ECHO_ON` variables below to match your use case
#  2. Run this script with the baud rate and the serial dev the device is connected
#     to, e.g.:
#       $ ./sneaky_urc 115200 /dev/ttyUSB0
#  4. run the test (e.g. make term)
#  5. inside the test console:
#    a) run the `init` command (e.g. init 0 115200)
#    b) run `sneaky_urc` command
#
# If the command echoing is enabled, you will miss the URCs, but the commands
# should work (e.g. the URC should not interfere with response parsing).
#
# If command echoing is enabled AND `MODULE_AT_URC` is defined, you should see
# *some* of the URCs being parsed.

import sys
import pexpect

baud = sys.argv[1]
ser = sys.argv[2]
tty = pexpect.spawn(f'picocom --baud {baud} {ser}')

# EOL sent by the device
EOL_IN = '\r'
# EOL to send back to the device
EOL_OUT = '\r\n'
# Command echoing enabled
ECHO_ON = False

CFUN_CMD = "AT+CFUN=1" + EOL_IN
CFUN_ERR_CMD = "AT+CFUN=8" + EOL_IN
CFUN_CME_CMD = "AT+CFUN=9" + EOL_IN
CEREG_CMD = "AT+CEREG?" + EOL_IN
USECMNG_CMD = "AT+USECMNG=0,0,\"cert\",128" + EOL_IN

while True:
    try:
        idx = tty.expect_exact([CFUN_CMD, CFUN_ERR_CMD, CFUN_CME_CMD, CEREG_CMD, USECMNG_CMD])
        if idx == 0:
            print(CFUN_CMD)
            tty.send(EOL_OUT + "+CSCON: 1" + EOL_OUT)
            if ECHO_ON:
                tty.send(CFUN_CMD)
            tty.send(EOL_OUT + "OK" + EOL_OUT)
        if idx == 1:
            print(CFUN_ERR_CMD)
            tty.send(EOL_OUT + "+CSCON: 1" + EOL_OUT)
            if ECHO_ON:
                tty.send(CFUN_ERR_CMD)
            tty.send(EOL_OUT + "ERROR" + EOL_OUT)
        if idx == 2:
            print(CFUN_CME_CMD)
            tty.send(EOL_OUT + "+CSCON: 1" + EOL_OUT)
            if ECHO_ON:
                tty.send(CFUN_CME_CMD)
            tty.send(EOL_OUT + "+CME ERROR:" + EOL_OUT)
        elif idx == 3:
            print(CEREG_CMD)
            tty.send(EOL_OUT + "+CSCON: 1" + EOL_OUT)
            if ECHO_ON:
                tty.send(CEREG_CMD)
            tty.send(EOL_OUT + "+CEREG: 0,1" + EOL_OUT)
            tty.send(EOL_OUT + "OK" + EOL_OUT)
        elif idx == 4:
            print(USECMNG_CMD)
            tty.send(EOL_OUT + "+CSCON: 1" + EOL_OUT)
            if ECHO_ON:
                tty.send(USECMNG_CMD)
            tty.send(">")
    except pexpect.EOF:
        print("ERROR: EOF")
    except pexpect.TIMEOUT:
        print("ERROR: TIMEOUT")
