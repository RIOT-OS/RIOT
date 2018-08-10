#!/usr/bin/env python3

# Copyright (C) 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Context swap race condition test application")
    child.expect_exact("Starting IRQ check thread")
    child.expect_exact(
        "Checking for working context swap (to detect false positives)... [Success]")
    child.expect_exact(
        "Checking for reset of swaps (to detect false positives)... [Success]")
    child.expect_exact("Checking for context swap race condition... [Success]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
