#!/usr/bin/env python3

# Copyright (C) 2020 iosabi
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('Testing Si1133 in blocking mode:')
    child.expect_exact('Result: OK')
    print('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
