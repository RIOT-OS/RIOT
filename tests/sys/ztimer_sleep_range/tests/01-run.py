#!/usr/bin/env python3

# Copyright (C) 2020 SSV Software Systems GmbH
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import time
from testrunner import run

US_PER_SEC = 1000000
INTERNAL_JITTER = 0.05
NUM_TEST_THREADS = 6

class InvalidTimeout(Exception):
    pass

def _test1(child):
    child.expect_exact("######################### TEST1:")
    child.expect(
        u"Running test (\\d+) times with (\\d+) distinct sleep time ranges"
    )
    RUNS = int(child.match.group(1))
    SLEEP_TIMES_NUMOF = int(child.match.group(2))

    try:
        for m in range(RUNS):
            for n in range(SLEEP_TIMES_NUMOF):
                child.expect(
                    u"Slept between (\\d+) us and (\\d+) us, "
                    u"actually slept (\\d+) us"
                )
                min_sleep = int(child.match.group(1))
                max_sleep = int(child.match.group(2))
                allowed_error = max_sleep * INTERNAL_JITTER
                actual_sleep = int(child.match.group(3))
                
                if not (min_sleep <= actual_sleep <= max_sleep + allowed_error):
                    raise InvalidTimeout(
                                    "Invalid sleep time %d us, expected between"
                                    " %d us and %d us" %
                                    (actual_sleep, min_sleep, max_sleep)
                                )
    except InvalidTimeout as e:
        print(e)

def _test2(child):
    child.expect_exact("######################### TEST2:")
    for i in range(NUM_TEST_THREADS - 1, -1, -1):
        child.expect(
            u"THREAD (\\d+): Slept between (\\d+) us and (\\d+) us, "
            u"actually slept (\\d+) us"
        )
    child.expect_exact("FINISHED")

def testfunc(child):
    _test1(child)
    _test2(child)


if __name__ == "__main__":
    sys.exit(run(testfunc))
