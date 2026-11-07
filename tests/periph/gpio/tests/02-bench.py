#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
import os
from testrunner import run


# Allow setting a specific port to test
PORT_UNDER_TEST = int(os.environ.get('PORT_UNDER_TEST') or 0)


def testfunc(child):
    for pin in range(0, 8):
        child.sendline("bench {} {}".format(PORT_UNDER_TEST, pin))
        child.expect(r" *nop loop: +(\d+)us  --- +(\d+\.\d+)us per call  --- +(\d+) calls per sec")
        child.expect(r" *gpio_set: +(\d+)us  --- +(\d+\.\d+)us per call  --- +(\d+) calls per sec")
        child.expect(r" *gpio_clear: +(\d+)us  --- +(\d+\.\d+)us per call  --- +(\d+) calls per sec")
        child.expect(r" *gpio_toggle: +(\d+)us  --- +(\d+\.\d+)us per call  --- +(\d+) calls per sec")
        child.expect(r" *gpio_read: +(\d+)us  --- +(\d+\.\d+)us per call  --- +(\d+) calls per sec")
        child.expect(r" *gpio_write: +(\d+)us  --- +(\d+\.\d+)us per call  --- +(\d+) calls per sec")
        child.expect_exact(" --- DONE ---")
        child.expect_exact(">")

    # TODO do some automated verification here? E.g. all pins should have the
    #      same timing?
    # TODO parse output data and put in some unified format?

    print("Benchmark was successful")


if __name__ == "__main__":
    sys.exit(run(testfunc))
