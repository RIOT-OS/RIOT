#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"SUCCESS: error code EHOSTUNREACH \((\d+) == (\d+)\)")
    assert child.match.group(1) == child.match.group(2)
    child.expect(r"SUCCESS: error code ENETUNREACH \((\d+) == (\d+)\)")
    assert child.match.group(1) == child.match.group(2)


if __name__ == "__main__":
    sys.exit(run(testfunc))
