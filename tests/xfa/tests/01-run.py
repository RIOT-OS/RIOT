#!/usr/bin/env python3

# Copyright (C) 2021 Freie Universität Berlin
#               2021 Inria
#               2021 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('Cross file array test')
    child.expect_exact('xfatest[2]:')
    child.expect_exact('[0] = 1, "xfatest1"')
    child.expect_exact('[1] = 2, "xfatest2"')
    child.expect_exact('xfatest_const[2]:')
    child.expect_exact('[0] = 123, "xfatest_const1"')
    child.expect_exact('[1] = 45, "xfatest_const2"')


if __name__ == "__main__":
    sys.exit(run(testfunc))
