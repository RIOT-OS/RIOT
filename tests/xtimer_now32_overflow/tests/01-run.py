#!/usr/bin/env python3

# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("[START]: xtimer_now32_overflow")
    child.expect_exact("[SUCCESS]: xtimer_now32_overflow")


if __name__ == "__main__":
    sys.exit(run(testfunc))
