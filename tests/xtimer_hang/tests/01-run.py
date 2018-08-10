#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("[START]")

    # due to timer inaccuracies, boards might not display exactly 100 steps, so
    # we accept 10% deviation
    for i in range(90):
        child.expect(u"Testing \( +\d+%\)")

    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
