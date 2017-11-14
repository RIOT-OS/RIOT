#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect_exact("xtimer_periodic_wakeup test application.")

    for i in range(256):
        child.expect("Testing interval \d+... \(now=\d+\)")
    for i in range(256):
        child.expect(" +\d+ diff=\d+")

    child.expect("Min/max error: \d+/\d+")
    child.expect_exact("Test complete.")

if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
