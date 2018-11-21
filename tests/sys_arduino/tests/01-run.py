#!/usr/bin/env python3

# Copyright (C) 2018 Federico Pellegrin <fede@evolware.org>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    # 1 Basic read+write test on serial with error command
    child.sendline("wrang")
    child.expect_exact("UNK")

    # 2 Test serial echo
    child.sendline("echo quite long string echoing on arduino module test")
    child.expect("ECHO: quite long string echoing on arduino module test")

    # 3 Test on various print base conversions
    child.sendline("numb 4242")
    child.expect_exact("4242 4242 1092 10222")

    # 4 Test if time is running and being printed
    child.sendline("time")
    child.expect("OK END")


if __name__ == "__main__":
    sys.exit(run(testfunc))
