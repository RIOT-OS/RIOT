#!/usr/bin/env python3

# Copyright (C) 2021 TUBA Freiberg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("starting ztimers")
    child.expect_exact("waiting for locks")
    child.expect_exact("USEC")
    child.expect_exact("MSEC")
    child.expect_exact("SEC")
    child.expect_exact("SUCCESS!")


if __name__ == "__main__":
    sys.exit(run(testfunc))
