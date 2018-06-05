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
    child.expect("This test will call xtimer_usleep for values from (\d+) us down to (\d+) us.")
    sleep_max = int(child.match.group(1))
    sleep_min = int(child.match.group(2))
    delay = sleep_max - sleep_min

    child.expect("Expected delay margin is (\d+) us.")
    sleep_margin = int(child.match.group(1))

    for delay in range(sleep_max, sleep_min, -1):
        try:
            child.expect("\w+: Slept for \d+ us not in range \[\d+, \d+\] us.",
                         timeout=20)

        except pexpect.TIMEOUT:
            print("xtimer stuck when trying to sleep %d us or failed.".format(delay))
            print("[FAILED]")
            exit(1)

    child.expect("Slept for \d+ us expected \d+ us.")
    child.expect("((Sleep delay margin was not kept for \d+ times.)|(\[SUCCESS\]))")
    msg = child.match.group(1)
    if msg != "[SUCCESS]" :
        child.expect("[FAILED]")
        exit(1)

if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
