#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Hello blob!")
    child.expect_exact("Hello blob_subdir!")
    child.expect_exact("0x00")
    child.expect_exact("0x01")
    child.expect_exact("0x02")
    child.expect_exact("0x03")
    child.expect_exact("0xFF")


if __name__ == "__main__":
    sys.exit(run(testfunc))
