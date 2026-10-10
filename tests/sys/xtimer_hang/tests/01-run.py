#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("[START]")

    # due to timer inaccuracies, boards might not display exactly 100 steps, so
    # we accept 10% deviation
    for i in range(90):
        child.expect(r"Testing \( +\d+%\)")

    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
