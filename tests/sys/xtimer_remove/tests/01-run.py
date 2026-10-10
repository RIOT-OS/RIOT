#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

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
