#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import time

from testrunner import run

# It's assumed that the same APPEUI is used for abp and otaa
DEVEUI_ABP = os.getenv('DEVEUI_ABP')
DEVEUI_OTA = os.getenv('DEVEUI_OTA')
APPEUI = os.getenv('APPEUI')
APPKEY = os.getenv('APPKEY')
DEVADDR = os.getenv('DEVADDR')
NWKSKEY = os.getenv('NWKSKEY')
APPSKEY = os.getenv('APPSKEY')

# Default to things network RX2_DR
TEST_RX2_DR = os.getenv('RX2_DR', 3)

# Theoretical duty cycling timeoff for EU863-870
# https://www.semtech.com/uploads/documents/LoraDesignGuide_STD.pdf#page=7
TEST_DATA_RATES = {"0": 164.6, "3": 20.6, "5": 6.2}

# Dummy Message
MSG = "This is RIOT"


def _send_line_echo(child, line):
    child.sendline(line)
    child.expect_exact(line)
    child.expect_exact(">")


def _send_line(child, line, expect_line):
    child.sendline(line)
    child.expect_exact(expect_line)
    child.expect_exact(">")


def _reset_config(child):
    # Start with a clean config
    child.sendline("loramac erase")
    child.expect("loramac erase")
    child.expect_exact(">")
    child.sendline("reboot")
    child.expect_exact("reboot")
    child.expect_exact("All up, running the shell now")
    child.expect_exact(">")


def _check_eeprom(child):
    # Check if eeprom is supported
    child.sendline("loramac help")
    child.expect(r'Usage: loramac \<([\w+\|?]+)\>')
    return (len(child.match.group(1).split('|')) == 7)


def _reboot(child, join):
    if join == "abp":
        child.sendline("loramac get ul_cnt")
        child.expect(r'Uplink Counter: (\d+)')
        uplink_counter = int(child.match.group(1))
    child.sendline("reboot")
    child.expect_exact("All up, running the shell now")
    child.expect_exact(">")
    if join == "abp":
        _send_line_echo(child, "loramac set ul_cnt {}".format(uplink_counter))


def _loramac_setup(child, join):
    if join == "abp":
        _send_line_echo(child, "loramac set deveui {}".format(DEVEUI_ABP))
        _send_line_echo(child, "loramac set appeui {}".format(APPEUI))
        _send_line_echo(child, "loramac set devaddr {}".format(DEVADDR))
        _send_line_echo(child, "loramac set nwkskey {}".format(NWKSKEY))
        _send_line_echo(child, "loramac set appskey {}".format(APPSKEY))
        _send_line_echo(child, "loramac set rx2_dr {}".format(TEST_RX2_DR))
    else:
        _send_line_echo(child, "loramac set deveui {}".format(DEVEUI_OTA))
        _send_line_echo(child, "loramac set appeui {}".format(APPEUI))
        _send_line_echo(child, "loramac set appkey {}".format(APPKEY))


def loramac_tx_test(child, join):

    _reset_config(child)

    # test all data rates
    for key, time_off in TEST_DATA_RATES.items():
        # Setup keys and rx2_dr
        _loramac_setup(child, join)
        # Set DR and join
        _send_line_echo(child, "loramac set dr {}".format(key))
        child.sendline("loramac join {}".format(join))
        child.expect_exact(["Join procedure succeeded!",
                            "Warning: already joined!"])
        child.expect_exact(">")
        # Transmit cnf message
        child.sendline("loramac tx \"{}\" cnf 123".format(MSG))
        child.expect_exact("Received ACK from network", timeout=30)
        child.expect_exact("Message sent with success")
        child.expect_exact(">")
        # Wake-up just before time_off, fail to send
        time.sleep(time_off)
        # Send uncnf message with success
        child.sendline("loramac tx \"{}\" uncnf 42".format(MSG))
        child.expect_exact("Message sent with success")
        child.expect_exact(">")
        # Reboot node
        _reboot(child, join)


def test_task02(child):
    loramac_tx_test(child, "otaa")


def test_task03(child):
    loramac_tx_test(child, "abp")


def test_task04(child):
    # Erase eeprom
    _reset_config(child)

    # Verify start from erased state
    _send_line(child, "loramac get deveui", "DEVEUI: 0000000000000000")
    _send_line(child, "loramac get appeui", "APPEUI: 0000000000000000")
    _send_line(child, "loramac get appkey",
               "APPKEY: 00000000000000000000000000000000")
    _send_line(child, "loramac get devaddr", "DEVADDR: 00000000")
    _send_line(child, "loramac get nwkskey",
               "NWKSKEY: 00000000000000000000000000000000")
    _send_line(child, "loramac get appskey",
               "APPSKEY: 00000000000000000000000000000000")

    # Save and verify otaa keys
    _loramac_setup(child, "otaa")
    _send_line_echo(child, "loramac save")
    child.sendline("reboot")
    child.expect_exact("All up, running the shell now")
    child.expect_exact(">")
    _send_line(child, "loramac get deveui", "DEVEUI: {}".format(DEVEUI_OTA))
    _send_line(child, "loramac get appeui", "APPEUI: {}".format(APPEUI))
    _send_line(child, "loramac get appkey", "APPKEY: {}".format(APPKEY))
    _reset_config(child)

    # Save and verify abp keys
    _loramac_setup(child, "abp")
    _send_line_echo(child, "loramac save")
    child.sendline("reboot")
    child.expect_exact("All up, running the shell now")
    child.expect_exact(">")
    _send_line(child, "loramac get devaddr", "DEVADDR: {}".format(DEVADDR))
    _send_line(child, "loramac get nwkskey", "NWKSKEY: {}".format(NWKSKEY))
    _send_line(child, "loramac get appskey", "APPSKEY: {}".format(APPSKEY))


def testfunc(child):

    def run(func):
        if child.logfile == sys.stdout:
            func(child)
        else:
            try:
                func(child)
                print(".", end="", flush=True)
            except Exception as e:
                print("FAILED")
                raise e

    run(test_task02)
    run(test_task03)
    if(_check_eeprom(child)):
        run(test_task04)

    print("TEST PASSED")


if __name__ == "__main__":
    sys.exit(run(testfunc))
