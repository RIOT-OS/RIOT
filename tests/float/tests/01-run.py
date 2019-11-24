#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run

# It takes 35 seconds on wsn430, so add some margin
TIMEOUT = 45


def testfunc(child):
    child.expect_exact("Testing floating point arithmetic...")
    child.expect_exact("[SUCCESS]", timeout=TIMEOUT)


if __name__ == "__main__":
    sys.exit(run(testfunc))
