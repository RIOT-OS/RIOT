#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2018 Acutam Automation, LLC
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("EEPROM registry (eepreg) test routine")
    child.expect_exact("Testing new registry creation: reset check [SUCCESS]")
    child.expect_exact("Testing writing and reading entries: add write add read [SUCCESS]")
    child.expect_exact("Testing detection of conflicting size: add [SUCCESS]")
    child.expect_exact("Testing calculation of lengths: len len [SUCCESS]")
    child.expect_exact("Testing of successful data move after rm: rm read data [SUCCESS]")
    child.expect_exact("Testing of free space change after write: free add free [SUCCESS]")
    child.expect_exact("Testing of iteration over registry: iter bar foo [SUCCESS]")
    child.expect_exact("Tests complete!")


if __name__ == "__main__":
    sys.exit(run(testfunc))
