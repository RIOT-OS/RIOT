#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("ztimer benchmark application.\r\n")
    for i in range(13):
        child.expect(r"\s+[\w() _\+]+\s+\d+ / \d+ = \d+\r\n")

    child.expect_exact("done.\r\n")


if __name__ == "__main__":
    sys.exit(run(testfunc))
