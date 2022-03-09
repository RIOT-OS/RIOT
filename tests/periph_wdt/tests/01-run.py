#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import os
import pexpect
import time
import subprocess
from testrunner import run

# nucleo-l152re is the default platform for this test
BOARD = os.environ.get("BOARD", "nucleo-l152re")
BOARD_CPU = subprocess.check_output([
    'make',
    'info-debug-variable-CPU',
    'BOARD={}'.format(BOARD),
    '--no-print-directory']).strip()

# We test only up to 10ms, with smaller times mcu doesn't have time to
# print system time before resetting
# cc2538 only supports 4 intervals [2ms, 16ms, 250ms, 1s]
if BOARD_CPU.decode("utf-8") == 'cc2538':
    reset_times_ms = [16, 250, 1000]
else:
    reset_times_ms = [128, 512, 1024, 8192]

# We don't check for accuracy, only order of magnitude. Some MCU use an
# an internal un-calibrated clock as reference which can deviate in
# more than 50% from theoretical values (e.g STM32 board CLOCK_LSI)
error_margin = 0.5


def get_reset_time(child):
    reset_time = 0
    try:
        start = time.time()
        timeout = 10  # seconds
        while time.time() < start + timeout:
            child.expect(r"reset time: (\d+) us", timeout=1)
            reset_time = int(child.match.group(1))
        else:
            # timeout
            return reset_time
    except pexpect.TIMEOUT:
        return reset_time


def testfunc(child):
    child.expect_exact(">")
    child.sendline("range")
    child.expect(r"lower_bound: (\d+) upper_bound: (\d+)\s*\r\n",
                 timeout=1)
    wdt_lower_bound = int(child.match.group(1))
    wdt_upper_bound = int(child.match.group(2))

    for rst_time in reset_times_ms:
        child.expect_exact(">")
        child.sendline("setup 0 {}".format(rst_time))
        if rst_time < wdt_lower_bound or rst_time > wdt_upper_bound:
            child.expect_exact("invalid time, see \"range\"", timeout=1)
        else:
            child.expect_exact(">")
            child.sendline("startloop")
            child.expect(r"start time: (\d+) us", timeout=1)
            start_time_us = int(child.match.group(1))
            reset_time_us = get_reset_time(child)
            wdt_reset_time = (reset_time_us - start_time_us) / 1e3

            if wdt_reset_time < rst_time*(1 - error_margin) or \
               wdt_reset_time > rst_time*(1 + error_margin):
                print("FAILED target time: {}[ms], actual_time: {}[ms]"
                      .format(rst_time, wdt_reset_time))
                sys.exit(-1)
            else:
                print("PASSED target time: {}[ms], actual_time: {}[ms]"
                      .format(rst_time, wdt_reset_time))

    print("TEST PASSED")


if __name__ == "__main__":
    sys.exit(run(testfunc, echo=False))
