#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2021 TUBA Freiberg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("starting threads")
    child.expect_exact("main is going to sleep")
    child.expect_exact("wakeup main")
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
