#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-FileCopyrightText: 2017 HAW Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"MAIN: reply from T-\d+")
    child.expect(r"MAIN: \d+! = \d+")
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
