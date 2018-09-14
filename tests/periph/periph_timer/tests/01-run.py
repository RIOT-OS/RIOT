#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect('Available timers: (\d+)')
    timers_num = int(child.match.group(1))
    for timer in range(timers_num):
        child.expect_exact('Testing TIMER_{}'.format(timer))
        child.expect_exact('TIMER_{}: initialization successful'.format(timer))
        child.expect_exact('TIMER_{}: stopped'.format(timer))
        child.expect_exact('TIMER_{}: starting'.format(timer))
    child.expect('TEST SUCCEEDED')


if __name__ == "__main__":
    sys.exit(run(testfunc))
