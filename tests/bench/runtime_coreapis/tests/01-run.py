#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2018 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

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
    child.expect(r"\{'BENCH_CLIST_SORT_TEST_NODES': (\d+)\}")
    clist_nodes = int(child.match.group(1))
    len = 4
    while len <= clist_nodes:
        child.expect(BENCHMARK_REGEXP.format(func=f"clist_sort, #{len}, rev", timeout=TIMEOUT))
        child.expect(BENCHMARK_REGEXP.format(func=f"clist_sort, #{len}, prng", timeout=TIMEOUT))
        child.expect(BENCHMARK_REGEXP.format(func=f"clist_sort, #{len}, sort", timeout=TIMEOUT))
        child.expect(BENCHMARK_REGEXP.format(func=f"clist_sort, #{len}, alm.srt", timeout=TIMEOUT))
        len = len << 1
    child.expect_exact('[SUCCESS]')


if __name__ == "__main__":
    sys.exit(run(testfunc))
