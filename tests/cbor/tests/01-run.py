#!/usr/bin/env python3

# Copyright (C) 2017 OTA keys S.A.
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run

ACCEPTED_ERROR = 20


def testfunc(child):
    child.expect_exact('Data:')
    child.expect_exact('(uint64_t, 1)')
    child.expect_exact('(uint64_t, 2)')
    child.expect_exact('(uint64_t, 3)')
    child.expect_exact('(int64_t, -5)')
    child.expect_exact('(bool, 1)')
    child.expect_exact('(float, 1.099609)')
    child.expect_exact('(float, 1.500000)')
    child.expect_exact('(double, 2.000000)')
    child.expect_exact('(byte string, "abc")')
    child.expect_exact('(unicode string, "def")')
    child.expect_exact('(array, length: 2)')
    child.expect_exact('  (uint64_t, 0)')
    child.expect_exact('  (uint64_t, 1)')
    child.expect_exact('(array, length: [indefinite])')
    child.expect_exact('  (uint64_t, 10)')
    child.expect_exact('  (uint64_t, 11)')
    child.expect_exact('(map, length: 2)')
    child.expect_exact(' (uint64_t, 1)')
    child.expect_exact('  (byte string, "1")')
    child.expect_exact(' (uint64_t, 2)')
    child.expect_exact('  (byte string, "2")')
    child.expect_exact('(map, length: [indefinite])')
    child.expect_exact(' (uint64_t, 10)')
    child.expect_exact('  (byte string, "10")')
    child.expect_exact(' (uint64_t, 11)')
    child.expect_exact('  (byte string, "11")')
    child.expect(r'\(tag: 0, date/time string: "[\w :]+"\)')
    child.expect(r'\(tag: 1, date/time epoch: \d+\)')
    child.expect_exact('(unsupported, 0xC2')
    child.expect_exact(')')
    child.expect_exact('(byte string, "1")')

    print("All tests successful")


if __name__ == "__main__":
    sys.exit(run(testfunc))
