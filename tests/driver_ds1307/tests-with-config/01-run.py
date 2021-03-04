#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys

import pexpect

from testrunner import run, run_check_unittests


def testfunc(child):
    res = child.expect([
        r"error: unable to initialize RTC \[I2C initialization error\]",
        pexpect.TIMEOUT,
    ])
    if res == 1:
        run_check_unittests()


if __name__ == "__main__":
    sys.exit(run(testfunc))
