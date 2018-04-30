#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect(u"xtimer_periodic_wakeup test application runs (\d+)")
    SLEEP_TIMES_NUMOF = int(child.match.group(1))

    for i in range(SLEEP_TIMES_NUMOF):
        child.expect(u"Testing interval \d+ ticks \(now=\d+\)")
    for i in range(SLEEP_TIMES_NUMOF):
        child.expect(u"Interval \d+ ticks diff=\d+")

    child.expect(u"Min/max error: \d+/\d+")
    child.expect_exact("Test complete.")
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTTOOLS'], 'testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
