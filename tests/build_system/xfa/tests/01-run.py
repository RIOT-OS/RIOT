#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2021 Freie Universität Berlin
# SPDX-FileCopyrightText: 2021 Inria
# SPDX-FileCopyrightText: 2021 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('Cross file array test')
    child.expect_exact('xfatest[4]:')
    child.expect_exact("[0] = 1, \"xfatest1\", 'a'")
    child.expect_exact("[1] = 2, \"xfatest2\", 'b'")
    child.expect_exact("[2] = 3, \"xfatest3\", 'c'")
    child.expect_exact("[3] = 4, \"xfatest4\", 'd'")
    child.expect_exact('xfatest_const[4]:')
    child.expect_exact("[0] = 123, \"xfatest_const1\", 'a'")
    child.expect_exact("[1] = 45, \"xfatest_const2\", 'b'")
    child.expect_exact("[2] = 42, \"xfatest_const3\", 'c'")
    child.expect_exact("[3] = 44, \"xfatest_const4\", 'd'")


if __name__ == "__main__":
    sys.exit(run(testfunc))
