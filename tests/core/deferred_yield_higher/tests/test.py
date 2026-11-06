#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('1. post yield')
    child.expect_exact('2. second thread scheduled')
    child.expect_exact('3. post irq enable')


if __name__ == "__main__":
    sys.exit(run(testfunc))
