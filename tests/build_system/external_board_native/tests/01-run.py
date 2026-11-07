#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
import os
from testrunner import run


def testfunc(child):
    board = os.environ['BOARD']
    child.expect_exact("Hello World!")
    child.expect_exact(f"You are running RIOT on a(n) {board} board.")
    child.expect_exact("THIS_BOARD_IS external_native")
    child.expect_exact("This board is 'An external extended native")
    print("Test successful!!")


if __name__ == "__main__":
    sys.exit(run(testfunc))
