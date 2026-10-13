#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2021 TUBA Freiberg
# SPDX-License-Identifier: LGPL-2.1-only

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
