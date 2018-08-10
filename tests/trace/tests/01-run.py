#!/usr/bin/env python3

# Copyright (C) 2016 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"TRACE_SIZE: (\d+)")
    trace_size = int(child.match.group(1))
    for i in range(trace_size):
        child.expect("0x[0-9a-f]{7,8}")

    print("All tests successful")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1, echo=True, traceback=True))
