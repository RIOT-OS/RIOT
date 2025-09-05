#! /usr/bin/env python3
#
# Copyright (C) 2025 TU Dresden
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run

def testfunc(child):
    child.expect_exact("[mock_nrfble] initialization successful")
    child.expect_exact("Data item 0 sent:")
    # TODO: check if this is actually correct
    child.expect("00000000  42  ..  ..  ..  ..  ..  ..  ..  02  01  06  05  09  54  65  73")
    child.expect_exact("00000010  74  10  16  D2  FC  40  00  42  02  C4  09  03  7C  15  04  DA")
    child.expect_exact("00000020  58  8F  00  00  00  00  00")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=6))
