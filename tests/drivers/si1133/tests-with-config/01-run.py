#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 iosabi
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('Testing Si1133 in blocking mode:')
    child.expect_exact('Result: OK')
    print('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
