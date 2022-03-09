#!/usr/bin/env python3

# Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


ADJUST_SET_MARGIN = 1
ADJUST_SLEEP_MARGIN = 1


def testfunc(child):
    child.expect(r"ZTIMER_USEC->adjust_set = (\d+)\r\n")
    auto_adjust_set = int(child.match.group(1))
    child.expect(r"ZTIMER_USEC->adjust_sleep = (\d+)\r\n")
    auto_adjust_sleep = int(child.match.group(1))
    child.expect(r"min=-?\d+ max=-?\d+ avg_diff=\d+\r\n")
    child.expect(r"min=-?\d+ max=-?\d+ avg_diff=\d+\r\n")
    child.expect(r"CONFIG_ZTIMER_USEC_ADJUST_SET\s+(\d+)\r\n")
    adjust_set = int(child.match.group(1))
    child.expect(r"CONFIG_ZTIMER_USEC_ADJUST_SLEEP\s+(\d+)\r\n")
    adjust_sleep = int(child.match.group(1))
    assert auto_adjust_set >= adjust_set - ADJUST_SET_MARGIN
    assert auto_adjust_sleep >= adjust_sleep - ADJUST_SLEEP_MARGIN


if __name__ == "__main__":
    sys.exit(run(testfunc))
