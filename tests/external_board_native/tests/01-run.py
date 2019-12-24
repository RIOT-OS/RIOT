#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Hello World!")
    child.expect_exact("You are running RIOT on a(n) native board.")
    child.expect_exact("THIS_BOARD_IS external_native")
    child.expect_exact("This board is 'An external extended native")
    print("Test successful!!")


if __name__ == "__main__":
    sys.exit(run(testfunc))
