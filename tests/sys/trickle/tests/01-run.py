#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 HAW Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("[START]")

    for i in range(5):
        child.expect(u"now = \\d+, t = \\d+")

    child.expect_exact("[TRICKLE_RESET]")

    for i in range(7):
        child.expect(u"now = \\d+, t = \\d+")

    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
