#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("xtimer_remove test application.")
    child.expect_exact("Setting 3 timers, removing timer 0/3")
    child.expect_exact("timer 1 triggered.")
    child.expect_exact("timer 2 triggered.")
    child.expect_exact("Setting 3 timers, removing timer 1/3")
    child.expect_exact("timer 0 triggered.")
    child.expect_exact("timer 2 triggered.")
    child.expect_exact("Setting 3 timers, removing timer 2/3")
    child.expect_exact("timer 0 triggered.")
    child.expect_exact("timer 1 triggered.")
    child.expect_exact("test successful.")


if __name__ == "__main__":
    sys.exit(run(testfunc))
