#!/usr/bin/env python3

# Copyright (C) 2019 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import pexpect
from testrunner import run


TEST_INPUT = 'O'
RETRIES = 5
TIMEOUT = 3


def testfunc(child):
    expected_output = 'You entered \'{}\''.format(TEST_INPUT)
    for _ in range(0, RETRIES):
        child.sendline(TEST_INPUT)
        ret = child.expect_exact([expected_output, pexpect.TIMEOUT],
                                 timeout=TIMEOUT)
        if ret == 0:
            break
    else:
        child.expect_exact(expected_output)


if __name__ == "__main__":
    sys.exit(run(testfunc))
