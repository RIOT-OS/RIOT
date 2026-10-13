#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
import pexpect
from testrunner import run


def testfunc(child):
    child.expect(u"This test will call xtimer_usleep for values from \\d+ down to \\d+\r\n")

    i = 500

    while (i >= 0):
        try:
            child.expect(u"going to sleep \\d+ usecs...\r\n", timeout=3)
        except pexpect.TIMEOUT:
            print("xtimer stuck when trying to sleep %d usecs" % (i+1))
            print("[FAILED]")
            break
        i = i - 1

    child.expect(u"[SUCCESS]", timeout=3)


if __name__ == "__main__":
    sys.exit(run(testfunc))
