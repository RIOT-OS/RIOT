#!/usr/bin/env python3

# Copyright (C) 2020 Benjamin Valentin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.sendline("info")
    child.expect(r'mtd devices: (\d+)')
    mtd_numof = int(child.match.group(1))
    for dev in range(mtd_numof):
        child.sendline("test " + str(dev))
        child.expect_exact("[START]")
        child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
