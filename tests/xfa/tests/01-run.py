#!/usr/bin/env python3

# Copyright (C) 2018 Gaëtan Harter <gaetan.harter@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect_exact('Cross file array test')
    child.expect_exact('xfatest[2]:')
    child.expect_exact('[0] = 1, "xfatest1"')
    child.expect_exact('[1] = 2, "xfatest2"')
    child.expect_exact('xfatest_const[2]:')
    child.expect_exact('[0] = 123, "xfatest_const1"')
    child.expect_exact('[1] = 45, "xfatest_const2"')


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTTOOLS'], 'testrunner'))
    from testrunner import run
    exit(run(testfunc))
