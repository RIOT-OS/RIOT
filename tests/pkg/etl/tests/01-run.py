#!/usr/bin/env python3

# Copyright (C) 2022 Jens Wetterich <jens@wetterich-net.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Test: is_same: 1, is_unsigned: 0, vector: cdeab, size: 55 -> valid")
    print("All tests successful")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1, echo=True, traceback=True))
