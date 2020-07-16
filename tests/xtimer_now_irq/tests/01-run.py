#!/usr/bin/env python3

#  Copyright (C) 2020 Freie Universität Berlin,
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>

import sys
from testrunner import run

TIMEOUT = 20


def testfunc(child):
    res = child.expect(['Nothing to do for 32 bit timers.\r\n',
                        'xtimer_now_irq test application.\r\n'])
    if res == 1:
        for _ in range(4):
            child.expect_exact("OK", timeout=TIMEOUT)
    child.expect_exact("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc))
