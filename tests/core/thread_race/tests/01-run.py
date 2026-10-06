#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
# SPDX-License-Identifier: LGPL-2.1-only

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
