#!/usr/bin/env python3

# Copyright (C) 2017 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    # 1st check for periodic 2s Hello World message, i.e., 2 output + 1 msg
    for _ in range(7):
        child.expect(r"sec=\d+ min=\d+ hour=\d+")
        child.expect(r"sec=\d+ min=\d+ hour=\d+")
        child.expect(r"now=\d+:\d+ -> every 2.0s: Hello World")
    # 2nd check for periodic 5s test message, i.e., 5 output + 1 msg
    for _ in range(3):
        child.expect(r"sec=\d+ min=\d+ hour=\d+")
        child.expect(r"sec=\d+ min=\d+ hour=\d+")
        child.expect(r"sec=\d+ min=\d+ hour=\d+")
        child.expect(r"sec=\d+ min=\d+ hour=\d+")
        child.expect(r"sec=\d+ min=\d+ hour=\d+")
        child.expect(r"now=\d+:\d+ -> every 5.0s: This is a Test")


if __name__ == "__main__":
    sys.exit(run(testfunc))
