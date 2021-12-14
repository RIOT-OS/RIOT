#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("ztimer benchmark application.\r\n")
    for i in range(13):
        child.expect(r"\s+[\w() _\+]+\s+\d+ / \d+ = \d+\r\n")

    child.expect_exact("done.\r\n")


if __name__ == "__main__":
    sys.exit(run(testfunc))
