#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("xtimer_periodic_wakeup test application.")

    for i in range(256):
        child.expect(r"Testing interval \d+... \(now=\d+\)")
    for i in range(256):
        child.expect(r" +\d+ diff=\d+")

    child.expect(r"Min/max error: \d+/\d+")
    child.expect_exact("Test complete.")


if __name__ == "__main__":
    sys.exit(run(testfunc))
