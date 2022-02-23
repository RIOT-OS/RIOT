#!/usr/bin/env python3

# Copyright (C) 2021 Christian Amsüss <chrysn@fsfe.org>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

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
