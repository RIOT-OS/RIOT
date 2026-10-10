#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8

# SPDX-FileCopyrightText: 2017 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
import time
from testrunner import run

US_PER_SEC = 1000000
EXTERNAL_JITTER = 0.15


class InvalidTimeout(Exception):
    pass


def testfunc(child):
    try:
        start_test = time.time()
        child.expect_exact("5 x usleep(i++ * 500000)")
        for i in range(5):
            child.expect_exact("wake up")
        child.expect_exact("5 x sleep(i++)")
        for i in range(5):
            child.expect_exact("wake up")
        child.expect_exact("DONE")
        testtime = (time.time() - start_test) * US_PER_SEC
        exp = sum(i * 500000 for i in range(5)) + \
            sum(i * US_PER_SEC for i in range(5))
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
