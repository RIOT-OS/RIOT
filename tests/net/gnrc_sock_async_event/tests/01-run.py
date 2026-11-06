#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

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
