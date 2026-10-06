#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.sendline("test")
    child.expect_exact("OK")


if __name__ == "__main__":
    sys.exit(run(testfunc))
