#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner

EVENT_NUM = 8
TIME_MULTIPLIER = 1000

def check_time(child, i):
    child.expect(r"event %d received after (\d+) us" % i)
    time = int(child.match.groups(1)[0])

    assert((time > (TIME_MULTIPLIER * i) - 250) and \
           (time < (TIME_MULTIPLIER * i) + 200))

def testfunc(child):
    child.expect_exact("Test extimer_add()")
    for i in range(EVENT_NUM):
        check_time(child, i)
    child.expect_exact("Test extimer_add() (reversed order)")
    for i in range(EVENT_NUM):
        check_time(child, i)
    child.expect_exact("Test extimer_del()")
    for i in range(0, EVENT_NUM, 2):
        check_time(child, i)
    child.expect_exact("ALL TESTS SUCCESSFUL")

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
