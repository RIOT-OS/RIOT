#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run

# For BOARD's with large amount of RAM allocating all chunks takes longer
# than 10s
ALLOCATION_TIMEOUT = 30
FREE_TIMEOUT = ALLOCATION_TIMEOUT


def testfunc(child):
    child.expect(r'CHUNK_SIZE: (\d+)\r\n')
    chunk_size = int(child.match.group(1))
    child.expect(r'NUMBER_OF_TESTS: (\d+)\r\n')
    number_of_tests = int(child.match.group(1))
    initial_allocations = 0
    for _ in range(number_of_tests):
        child.expect(r"Allocated {} Bytes at (0x)?[a-z0-9]+, total [a-z0-9]+\r\n"
                     .format(chunk_size))
        child.expect(r'Allocations count: (\d+)\r\n',
                     timeout=ALLOCATION_TIMEOUT)
        allocations = int(child.match.group(1))
        assert allocations > 0
        if initial_allocations == 0:
            initial_allocations = allocations
        assert initial_allocations == allocations
        child.expect(r"Free {} Bytes at (0x)?[a-z0-9]+, total [a-z0-9]+\r\n"
                     .format(chunk_size))
        child.expect(r'Free count: (\d+)\r\n', timeout=FREE_TIMEOUT)
        freed = int(child.match.group(1))
        assert freed == allocations
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
