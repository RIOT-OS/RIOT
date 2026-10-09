#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2022 Jens Wetterich <jens@wetterich-net.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Test: is_same: 1, is_unsigned: 0, vector: cdeab, size: 55 -> valid")
    print("All tests successful")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1, echo=True, traceback=True))
