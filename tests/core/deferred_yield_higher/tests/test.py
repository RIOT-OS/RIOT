#!/usr/bin/env python3

# Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('1. post yield')
    child.expect_exact('2. second thread scheduled')
    child.expect_exact('3. post irq enable')


if __name__ == "__main__":
    sys.exit(run(testfunc))
