#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2021 Christian Amsüss <chrysn@fsfe.org>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Mounted 1234")
    child.expect_exact("N(/const1)N(/const2)N(/const3)N(/const4)O")
    child.expect_exact("N(/const1)N(/const2)N(/const4)O")
    child.expect_exact("N(/const1)N(/const4)N(/const3)N(/const1)O")
    child.expect_exact("All unmounted")
    child.expect_exact("O")


if __name__ == "__main__":
    sys.exit(run(testfunc))
