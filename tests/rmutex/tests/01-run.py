#!/usr/bin/env python3

# Copyright (C) 2016 Theobroma Systems Design & Consulting GmbH
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# Author: Martin Elshuber <martin.elshuber@theobroma-systems.com>

import sys
from testrunner import run

NUM_THREADS = 5

# Make sure this matches this line in main.c
#  static const char depth[THREAD_NUMOF] = {5, 3, 3, 4, 5};
expected_lock_depth = [5, 3, 3, 4, 5]


def testfunc(child):
    # First collect the thread info how they are created
    # A number of lines with:
    #  T4 (prio 6): waiting on condition variable now
    thread_prios = {}
    lock_depth = {}
    for nr in range(NUM_THREADS):
        child.expect(r"T(\d+) \(prio (\d+), depth 0\): trying to lock rmutex now")
        thread_id = int(child.match.group(1))
        thread_prio = int(child.match.group(2))
        thread_prios[thread_id] = thread_prio
        lock_depth[thread_id] = expected_lock_depth[nr]

    pri_sorted = sorted(thread_prios, key=lambda x: thread_prios[x] * 1000 + x)
    for T in pri_sorted:
        for depth in range(lock_depth[T]):
            child.expect_exact("T{} (prio {}, depth {}): locked rmutex now"
                               .format(T, thread_prios[T], depth))


if __name__ == "__main__":
    sys.exit(run(testfunc))
