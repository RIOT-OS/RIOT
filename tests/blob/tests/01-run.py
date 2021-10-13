#!/usr/bin/env python3

# Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

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
