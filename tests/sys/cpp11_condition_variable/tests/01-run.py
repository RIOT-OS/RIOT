#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("************ C++ condition_variable test ***********")
    child.expect_exact("Wait with predicate and notify one ...")
    child.expect_exact("Done")
    child.expect_exact("Wait and notify all ...")
    child.expect_exact("Done")
    child.expect_exact("Wait for ...")
    child.expect_exact("Done")
    child.expect_exact("Wait until ...")
    child.expect_exact("Done")
    child.expect_exact("Bye, bye.")
    child.expect_exact("******************************************************")


if __name__ == "__main__":
    sys.exit(run(testfunc))
