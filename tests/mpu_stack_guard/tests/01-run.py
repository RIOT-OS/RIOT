#!/usr/bin/env python3

# Copyright (C) 2020 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("MPU Stack Guard Test\r\n")
    for _ in range(100):
        child.expect(r"counter =[ ]+\d+, SP = 0x[0-9a-f]+, canary = 0xdeadbeef")
    child.expect(r".*RIOT kernel panic:")
    child.expect_exact("MEM MANAGE HANDLER\r\n")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=10))
