#!/usr/bin/env python3
#
# SPDX-FileCopyrightText: 2026 Mert Cakir <mert-cakir@outlook.com>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("rp2350 uart fifo loopback test")
    child.expect_exact("PASS: 64 bytes round-tripped via external loopback")
    child.expect_exact("ALL TESTS PASSED")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=15))
