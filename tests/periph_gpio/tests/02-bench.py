#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect_exact("GPIO peripheral driver test")
    child.expect_exact(">")

    for pin in range(0, 8):
        child.sendline("bench 0 {}".format(pin))
        child.expect(r" *nop loop: +(\d+)us  --- +(\d+\.\d+)us per call")
        child.expect(r" *gpio_set: +(\d+)us  --- +(\d+\.\d+)us per call")
        child.expect(r" *gpio_clear: +(\d+)us  --- +(\d+\.\d+)us per call")
        child.expect(r" *gpio_toggle: +(\d+)us  --- +(\d+\.\d+)us per call")
        child.expect(r" *gpio_read: +(\d+)us  --- +(\d+\.\d+)us per call")
        child.expect(r" *gpio_write: +(\d+)us  --- +(\d+\.\d+)us per call")
        child.expect_exact(" --- DONE ---")
        child.expect_exact(">")

    # TODO do some automated verification here? E.g. all pins should have the
    #      same timing?
    # TODO parse output data and put in some unified format?

    print("Benchmark was successful")


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc, timeout=10))
