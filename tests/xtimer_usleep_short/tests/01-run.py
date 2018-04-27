#!/usr/bin/env python3

# Copyright (C) 2017 Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import pexpect


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

    child.expect(u"Slept for \\d+ expected \\d+")
    child.expect(u"[SUCCESS]", timeout=3)


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
