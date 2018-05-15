#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8

# Copyright (C) 2017 Francisco Acosta <francisco.acosta@inria.fr>
#               2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import time

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
        child.expect_exact(u"Please hit any key and then ENTER to continue")
        child.sendline(u"a")
        start_test = time.time()
        for m in range(RUNS):
            for n in range(SLEEP_TIMES_NUMOF):
                child.expect(u"(\\d+) (\\d+) slept for\\s+(\\d+) ticks \(\\s+(\\d+) us\) expected" +
                             "\\s+(\\d+) ticks, diff\\s+(\\d+) ticks")
                start = int(child.match.group(1))
                end = int(child.match.group(2))
                sleep_time = int(child.match.group(3))
                exp = int(child.match.group(5))
                lower_bound = exp - (exp * EXTERNAL_JITTER)
                upper_bound = exp + (exp * EXTERNAL_JITTER)
                time.sleep(0.001)

                if m == 0 and n == SLEEP_TIMES_NUMOF-1:
                    print("Increase value for XTIMER_SHOOT_OVERHEAD or "
                          "XTIMER_OVERHEAD %d" % (sleep_time-exp))
                elif m == 3 and n == 1:
                    print("Increase value for XTIMER_BACKOFF_OVERHEAD %d" % (sleep_time-exp))
                elif exp/10 < 0 and not (lower_bound < sleep_time < upper_bound):
                    raise InvalidTimeout("Invalid timeout %d, expected %d, xtimer " +
                                         "compared to host clock has to much deviation"
                                         % (sleep_time, exp))

                if end - start < exp:
                    raise InvalidTimeout("To big values %d < %d, xtimer fires too early."
                                         % (sleep_time, exp))

        testtime = (time.time() - start_test) * US_PER_SEC
        child.expect(u"Test ran for (\\d+) us")
        exp = int(child.match.group(1))
        lower_bound = exp - (exp * EXTERNAL_JITTER)
        upper_bound = exp + (exp * EXTERNAL_JITTER)
        if not (lower_bound < testtime < upper_bound):
            raise InvalidTimeout("Host timer measured %d us (client measured %d us)"
                                 % (testtime, exp))
    except InvalidTimeout as e:
        print(e)
        sys.exit(1)


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
