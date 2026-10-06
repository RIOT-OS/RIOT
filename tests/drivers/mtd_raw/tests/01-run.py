#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Benjamin Valentin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.sendline("mtd info")
    child.expect(r'mtd devices: (\d+)')
    mtd_numof = int(child.match.group(1))
    for dev in range(mtd_numof):
        child.sendline("test " + str(dev))
        child.expect_exact("[START]")
        child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
