#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8

# Copyright (C) 2017 Francisco Acosta <francisco.acosta@inria.fr>
#               2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import time
from testrunner import run


US_PER_SEC = 1000000
INTERNAL_JITTER = 0.05
EXTERNAL_JITTER = 0.15


class InvalidTimeout(Exception):
    pass


def testfunc(child):
    child.expect(u"Running test (\\d+) times with (\\d+) distinct sleep times")
    RUNS = int(child.match.group(1))
    SLEEP_TIMES_NUMOF = int(child.match.group(2))
    try:
        start_test = time.time()
        for m in range(RUNS):
            for n in range(SLEEP_TIMES_NUMOF):
                child.expect(u"Slept for (\\d+) us \\(expected: (\\d+) us\\) Offset: (-?\\d+) us")
                sleep_time = int(child.match.group(1))
                exp = int(child.match.group(2))
                upper_bound = exp + (exp * INTERNAL_JITTER)
                if not (exp <= sleep_time < upper_bound):
                    delta = (upper_bound-exp)
                    error = min(upper_bound-sleep_time, sleep_time-exp)
                    raise InvalidTimeout("Invalid timeout %d, expected %d < timeout < %d"
                                         "\nHost max error\t%d\nerror\t\t%d" %
                                         (sleep_time, exp, upper_bound,
                                          delta, error))
        testtime = (time.time() - start_test) * US_PER_SEC
        child.expect(u"Test ran for (\\d+) us")
        exp = int(child.match.group(1))
        lower_bound = exp - (exp * EXTERNAL_JITTER)
        upper_bound = exp + (exp * EXTERNAL_JITTER)
        if not (lower_bound < testtime < upper_bound):
            raise InvalidTimeout("Host timer measured %d us (client measured %d us)" %
                                 (testtime, exp))
    except InvalidTimeout as e:
        print(e)
        sys.exit(1)


if __name__ == "__main__":
    sys.exit(run(testfunc))
