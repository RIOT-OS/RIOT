#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("UDP event triggered: 0030")
    child.expect_exact("Received UDP packet from [fe80::2]:38663:")
    child.expect_exact("00000000  01  23  45  67  89  AB  CD  EF")
    child.expect_exact("UDP message successfully sent")
    child.expect_exact("IP event triggered: 0030")
    child.expect_exact("Received IP packet from [fe80::2]:")
    child.expect_exact("00000000  01  23  45  67  89  AB  CD  EF")
    child.expect_exact("IP message successfully sent")


if __name__ == "__main__":
    sys.exit(run(testfunc))
