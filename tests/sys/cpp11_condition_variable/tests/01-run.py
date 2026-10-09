#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-FileCopyrightText: 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
# SPDX-License-Identifier: LGPL-2.1-only

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
