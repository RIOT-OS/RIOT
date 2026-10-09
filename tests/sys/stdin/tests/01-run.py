#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Alexandre Abadie <alexandre.abadie@inria.fr>
# SPDX-License-Identifier: LGPL-2.1-only

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
