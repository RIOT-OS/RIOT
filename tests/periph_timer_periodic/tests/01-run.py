#!/usr/bin/env python3

# Copyright (C) 2020 Benjamin Valentin <benpicco@beuth-hochschule.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import time
from testrunner import run


PRECISION = float(os.getenv("TEST_PERIPH_TIMER_PERIODIC_PRECISION", "0"))


def testfunc(child):
    child.expect_exact('TEST START')
    while True:
        child.expect(r"Running iteration (\d+) of (\d+)\r\n")
        iteration = int(child.match.group(1))
        last_iteration = int(child.match.group(2))
        start = time.time()
        for i in range(12):
            child.expect(r"\[(\d+)\] tick\r\n")
            chan = int(child.match.group(1))
            assert chan == 0, "Only channel 0 should tick"
        child.expect("Cycles:\r\n")
        end = time.time()
        # test should run 10 cycles with 25ms each
        elapsed = end - start
        assert elapsed > 0.25 * (1 - PRECISION), "=< 0.25s ({})".format(elapsed)
        assert elapsed < 0.40 * (1 + PRECISION), "=> 0.40s ({})".format(elapsed)
        if iteration == last_iteration:
            break
    child.expect_exact('TEST SUCCEEDED')


if __name__ == "__main__":
    sys.exit(run(testfunc))
