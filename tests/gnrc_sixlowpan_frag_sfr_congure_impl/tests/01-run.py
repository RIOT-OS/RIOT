#!/usr/bin/env python3

# Copyright (C) 2021 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.


import sys
from testrunner import run


def testfunc(child):
    child.sendline('udp send fe80::1 1337 abcde')
    child.expect_exact('Success: sent 5 byte(s) to [fe80::1]:1337')


if __name__ == "__main__":
    sys.exit(run(testfunc))
