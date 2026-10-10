#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-FileCopyrightText: 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("************ C++ thread test ***********")
    child.expect_exact("Creating one thread and passing an argument ...")
    child.expect_exact("Done")
    child.expect_exact("Creating detached thread ...")
    child.expect_exact("Done")
    child.expect_exact("Join on 'finished' thread ...")
    child.expect_exact("Done")
    child.expect_exact("Join on 'running' thread ...")
    child.expect_exact("Done")
    child.expect_exact("Testing sleep_for ...")
    child.expect_exact("Done")
    child.expect_exact("Testing sleep_until ...")
    child.expect_exact("Done")
    child.expect_exact("Swapping two threads ...")
    child.expect_exact("Done")
    child.expect_exact("Move constructor ...")
    child.expect_exact("Done")
    child.expect_exact("Bye, bye.")
    child.expect_exact("******************************************")


if __name__ == "__main__":
    sys.exit(run(testfunc))
