#!/usr/bin/env python3

# Copyright (C) 2017 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("[START]")

    for i in range(5):
        child.expect(u"now = \d+, prev_now = \d+, diff = \d+")

    child.expect_exact("[TRICKLE_RESET]")

    for i in range(7):
        child.expect(u"now = \d+, prev_now = \d+, diff = \d+")

    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
