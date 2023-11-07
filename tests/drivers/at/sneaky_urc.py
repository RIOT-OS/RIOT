#!/usr/bin/env python3

# 1. Adapt the `eol_in`, `eol_out`, `echo` variables below to match your use case
# 2. Bind this script to the serial line the device is connected at:
#   $ ./sneaky_urc /dev/ttyUSB0
# 4. run the test (e.g. make term)
# 5. inside the test console:
#   a) run the `init` command (e.g. init 0 115200)
#   b) run `sneaky_urc` command
# 
# This script sends a "sneaky URC", an URC that happens exactly after the
# command is received, but before the first line of answer is sent. This is possible
# behaviour at least on the U-Blox LTE modules.
#
# If the command echoing is enabled, you will miss the URCs, but the commands
# should work (e.g. the URC should not interfere with response parsing).
#
# If command echoing is enabled AND `MODULE_AT_URC` is defined, you should see 
# the URCs being parsed.

import sys
import pexpect

ser = sys.argv[1]
tty = pexpect.spawn(f'picocom --baud 115200 {ser}')

# EOL sent by the device
eol_in = '\r'
# EOL to send back to the device
eol_out = '\r\n'
# Command echoing enabled
echo = False

cfun_c = "AT+CFUN=1" + eol_in
cfun_err_c = "AT+CFUN=8" + eol_in
cfun_cme_c = "AT+CFUN=9" + eol_in
cereg_c = "AT+CEREG?" + eol_in
usecmng_c = "AT+USECMNG=0,0,\"cert\",128" + eol_in

while True:
    try:
        idx = tty.expect_exact([cfun_c, cfun_err_c, cfun_cme_c, cereg_c, usecmng_c])
        if idx == 0:
            print(cfun_c)
            tty.send(eol_out + "+CSCON: 1" + eol_out)
            if echo:
                tty.send(cfun_c)
            tty.send(eol_out + "OK" + eol_out)
        if idx == 1:
            print(cfun_err_c)
            tty.send(eol_out + "+CSCON: 1" + eol_out)
            if echo:
                tty.send(cfun_err_c)
            tty.send(eol_out + "ERROR" + eol_out)
        if idx == 2:
            print(cfun_cme_c)
            tty.send(eol_out + "+CSCON: 1" + eol_out)
            if echo:
                tty.send(cfun_cme_c)
            tty.send(eol_out + "+CME ERROR:" + eol_out)
        elif idx == 3:
            print(cereg_c)
            tty.send(eol_out + "+CSCON: 1" + eol_out)
            if echo:
                tty.send(cereg_c)
            tty.send(eol_out + "+CEREG: 0,1" + eol_out)
            tty.send(eol_out + "OK" + eol_out)
        elif idx == 4:
            print(usecmng_c)
            tty.send(eol_out + "+CSCON: 1" + eol_out)
            if echo:
                tty.send(usecmng_c)
            tty.send(">")
    except pexpect.EOF:
        print("ERROR: EOF")
    except pexpect.TIMEOUT:
        print("ERROR: TIMEOUT")