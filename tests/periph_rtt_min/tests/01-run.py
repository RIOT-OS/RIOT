#!/usr/bin/env python3

# Copyright (C) 2020 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

from testrunner import run

SAMPLES = int(os.getenv("SAMPLES", "1024"))


def testfunc(child):
    for _ in range(0, SAMPLES):
        child.expect_exact('.')
    child.expect(r'RTT_MIN_OFFSET for [a-zA-Z\-\_0-9]+: \d+')


if __name__ == "__main__":
    sys.exit(run(testfunc))
