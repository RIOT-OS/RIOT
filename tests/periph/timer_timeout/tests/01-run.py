#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"Available timers: (\d+)")
    timers_num = int(child.match.group(1))
    for timer in range(timers_num):
        child.expect_exact(r"Testing TIMER_DEV(%u)" % (timer, ))
        child.expect(r"TIMER_DEV\(%u\) running at \d+ Hz" % (timer, ))
    child.expect('TEST SUCCEEDED')


if __name__ == "__main__":
    sys.exit(run(testfunc))
