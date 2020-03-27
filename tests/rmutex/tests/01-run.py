#!/usr/bin/env python3

# Copyright (C) 2016 Theobroma Systems Design & Consulting GmbH
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# Author: Martin Elshuber <martin.elshuber@theobroma-systems.com>

import sys
from testrunner import run


thread_prio = {
        3:  6,
        4:  4,
        5:  5,
        6:  2,
        7:  4
        }

lock_depth = {
        3:  5,
        4:  3,
        5:  3,
        6:  4,
        7:  5
        }

timeouts = {
        3: 100000,
        4: 200000,
        5: 300000,
        6: 400000,
        7: 500000
        }


def thread_prio_sort(x):
    return thread_prio.get(x)*1000 + x


def testfunc(child):
    for k in thread_prio.keys():
        child.expect_exact("T{} (prio {}, depth 0): trying to lock rmutex now"
                           .format(k, thread_prio[k]))

    pri_sorted = sorted(thread_prio, key=thread_prio_sort)
    for T in pri_sorted:
        for depth in range(lock_depth[T]):
            child.expect_exact("T{} (prio {}, depth {}): locked rmutex now"
                               .format(T, thread_prio[T], depth))

    # xtimer_rmutex_lock_timeout() test
    for k in thread_prio.keys():
        child.expect_exact("T{} (prio {}, depth 0, timeout {}): trying to lock rmutex now"
                           .format(k, thread_prio[k], timeouts[k]))
    child.expect_exact("main: waiting 250ms")
    for k in range(3, 4):
        child.expect_exact("T{} (prio {}, depth 0): timeout"
                           .format(k, thread_prio[k]))
    child.expect_exact
    child.expect_exact("main: unlocking recursive mutex")
    pri_sorted = sorted({k: thread_prio[k] for k in (5, 6, 7)}, key=thread_prio_sort)
    for T in pri_sorted:
        for depth in range(3):
            child.expect_exact("T{} (prio {}, depth {}): locked rmutex now"
                               .format(T, thread_prio[T], depth))

    child.expect_exact("Test END, check the order of priorities above.")


if __name__ == "__main__":
    sys.exit(run(testfunc))
