#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect('TIMER_NUMOF=(\d+)')
    timer_numof = int(child.match.group(1))
    child.expect('TIMER_SPEED=(\d+)')
    timer_speed = int(child.match.group(1))
    for timer in range(timer_numof):
        child.expect_exact('*** Testing TIMER_{} ***'.format(timer))
        child.expect_exact('  timer_init({}, {}, cb, args) DONE'.format(timer, timer_speed))
    child.expect('TEST SUCCEEDED')


if __name__ == "__main__":
    sys.exit(run(testfunc))
