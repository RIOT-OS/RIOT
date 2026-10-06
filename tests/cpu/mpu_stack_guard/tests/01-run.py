#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Inria
# SPDX-License-Identifier: LGPL-2.1-only

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
