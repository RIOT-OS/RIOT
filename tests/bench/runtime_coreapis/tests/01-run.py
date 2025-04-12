#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


# The default timeout is not enough for this test on some of the slower boards
TIMEOUT = 30
BENCHMARK_REGEXP = r"\s+{func}:\s+\d+us\s+---\s+\d*\.*\d+us per call\s+---\s+\d+ calls per sec"


def testfunc(child):
    child.expect_exact('Runtime of Selected Core API functions')
    child.expect(BENCHMARK_REGEXP.format(func="nop loop"))
    child.expect(BENCHMARK_REGEXP.format(func=r"mutex_init\(\)"))
    child.expect(BENCHMARK_REGEXP.format(func="mutex lock/unlock"), timeout=TIMEOUT)
    child.expect(BENCHMARK_REGEXP.format(func=r"thread_flags_set\(\)"))
    child.expect(BENCHMARK_REGEXP.format(func=r"thread_flags_clear\(\)"))
    child.expect(BENCHMARK_REGEXP.format(func="thread flags set/wait any"), timeout=TIMEOUT)
    child.expect(BENCHMARK_REGEXP.format(func="thread flags set/wait all"), timeout=TIMEOUT)
    child.expect(BENCHMARK_REGEXP.format(func="thread flags set/wait one"), timeout=TIMEOUT)
    child.expect(BENCHMARK_REGEXP.format(func=r"msg_try_receive\(\)"), timeout=TIMEOUT)
    child.expect(BENCHMARK_REGEXP.format(func=r"msg_avail\(\)"))
    for _ in range(7):
        child.expect(BENCHMARK_REGEXP.format(func=r"clist_sort \(#\d+, worst\)", timeout=TIMEOUT))
        child.expect(BENCHMARK_REGEXP.format(func=r"clist_sort \(#\d+, avg\)", timeout=TIMEOUT))
    child.expect_exact('[SUCCESS]')


if __name__ == "__main__":
    sys.exit(run(testfunc))
