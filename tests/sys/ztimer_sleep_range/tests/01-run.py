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

class InvalidTimeout(Exception):
    pass

def testfunc(child):
    child.expect(r"Running test (\d+) times with (\d+) distinct sleep time ranges")
    RUNS = int(child.match.group(1))
    SLEEP_TIMES_NUMOF = int(child.match.group(2))

    try:
        for m in range(RUNS):
            for n in range(SLEEP_TIMES_NUMOF):
                child.expect(r"Slept between (\d+) us and (\d+) us, actually slept (\d+) us")
                min_sleep = int(child.match.group(1))
                max_sleep = int(child.match.group(2))
                actual_sleep = int(child.match.group(3))
                
                if not (min_sleep <= actual_sleep <= max_sleep):
                    raise InvalidTimeout("Invalid sleep time %d us, expected between %d us and %d us" %
                                         (actual_sleep, min_sleep, max_sleep))
    except InvalidTimeout as e:
        print(e)
        sys.exit(1)

if __name__ == "__main__":
    sys.exit(run(testfunc))
