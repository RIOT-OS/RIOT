#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("On-board button test")
    index = child.expect([
        r"\[FAILED\] no buttons available!",
        r" -- Available buttons: \d+"
    ])

    if index == 1:
        child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
