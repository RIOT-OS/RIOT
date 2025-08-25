#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect(r'MAX_NUMBER_BLOCKS: (\d+)\r\n')
    max_number_blocks = int(child.match.group(1))
    child.expect(r'NUMBER_OF_LOOPS: (\d+)\r\n')
    number_of_loops = int(child.match.group(1))
    child.expect(r'NUMBER_OF_TESTS: (\d+)\r\n')
    number_of_tests = int(child.match.group(1))
    for loop in range(number_of_loops):
        child.expect_exact("LOOP #{}:".format(loop + 1))
        for test in range(number_of_tests):
            child.expect_exact("TEST #{}:".format(test + 1))
            for i in range(max_number_blocks):
                child.expect(r'\({}, @@@@@@@\) Allocated \d+ Bytes at (0x)?[a-zA-Z0-9]+,'
                             r' total [0-9]+\r\n'.format(i))
            for i in range(max_number_blocks):
                child.expect(r'Free \({}\) \d+ Bytes at (0x)?[a-zA-Z0-9]+,'
                             ' total [0-9]+\r\n'.format(i))

    child.expect_exact("Extend and reduce tests")

    child.expect_exact("Memarray available: {}".format(max_number_blocks))
    child.expect_exact("Memarray available: {}".format(2 * max_number_blocks))
    child.expect_exact("Memarray reduction: 0 available: {}"
                       "".format(max_number_blocks))
    child.expect_exact("Memarray reduction: -1 available: {}"
                       "".format(max_number_blocks))
    child.expect_exact("Memarray reduction: 0 available: 0")
    child.expect_exact("Memarray available: {}".format(max_number_blocks - 1))
    child.expect_exact("Memarray reduction: -1 available: {}"
                       "".format(max_number_blocks - 1))

    child.expect_exact("Finishing")


if __name__ == "__main__":
    sys.exit(run(testfunc))
