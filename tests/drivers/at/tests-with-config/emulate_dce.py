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
#       $ ./emulate_dce.py 115200 /dev/ttyUSB0
#  4. run the test (e.g. make term)
#  5. inside the test console:
#    a) run the `init` command (e.g. init 0 115200)
#    b) run `emulate_dce` command
#
# If the command echoing is enabled, you will miss the URCs, but the commands
# should work (e.g. the URC should not interfere with response parsing).
#
# If command echoing is enabled AND `MODULE_AT_URC` is defined, you should see
# *some* of the URCs being parsed.
#
# The easiest way is to run this test on native, with an socat tty bridge:
#   a) run `$ socat -d -d pty,raw,echo=0 pty,raw,echo=0`
#      socat will display the pty endpoints it just created, e.g:
#         2024/01/23 10:24:56 socat[45360] N PTY is /dev/pts/9
#         2024/01/23 10:24:56 socat[45360] N PTY is /dev/pts/10
#   b) pick one pty endpoint (e.g. /dev/pts/9) and run this script:
#      `$ emulate_dce.py 115200 /dev/pts/9`
#   c) pick the other endpoint and run the compiled binary
#      ` $ ./bin/native/tests_at.elf -c /dev/pts/10 < tests-with-config/native_stdin`

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
ECHO_ON = True

CFUN_CMD = "AT+CFUN=1" + EOL_IN
CFUN_ERR_CMD = "AT+CFUN=8" + EOL_IN
CFUN_CME_CMD = "AT+CFUN=9" + EOL_IN
CEREG_CMD = "AT+CEREG?" + EOL_IN
USECMNG_CMD = "AT+USECMNG=0,0,\"cert\",128" + EOL_IN
GETTWOLINES_CMD = "AT+GETTWOLINES" + EOL_IN
PROMPTERROR_CMD = "AT+PROMPTERROR" + EOL_IN

while True:
    try:
        idx = tty.expect_exact([CFUN_CMD, CFUN_ERR_CMD, CFUN_CME_CMD, CEREG_CMD, USECMNG_CMD,
                                GETTWOLINES_CMD, PROMPTERROR_CMD])
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
            tty.send(EOL_OUT + "+CME ERROR: 666" + EOL_OUT)
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
        elif idx == 5:
            print(GETTWOLINES_CMD)
            if ECHO_ON:
                tty.send(GETTWOLINES_CMD)
            tty.send(EOL_OUT + "first_line" + EOL_OUT + "second_line" + EOL_OUT + "OK" + EOL_OUT)
        elif idx == 6:
            print(PROMPTERROR_CMD)
            if ECHO_ON:
                tty.send(PROMPTERROR_CMD)
            tty.send(EOL_OUT + "+CME ERROR: 1984" + EOL_OUT)
    except pexpect.EOF:
        print("ERROR: EOF")
    except pexpect.TIMEOUT:
        print("ERROR: TIMEOUT")
