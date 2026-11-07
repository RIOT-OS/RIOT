#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


# Use custom timeout, per test.
# test #3 requires ~150s to complete on samr30-xpro.
TIMEOUT = 180


def testfunc(child):
    for i in range(1, 6):
        child.expect(r"Running test #%d... done. Run time: \d+.\d{3} s" % i,
                     timeout=TIMEOUT)


if __name__ == "__main__":
    sys.exit(run(testfunc))
