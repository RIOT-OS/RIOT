#!/usr/bin/env python3

# Copyright (C) 2016 Theobroma Systems Design & Consulting GmbH
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# Author: Martin Elshuber <martin.elshuber@theobroma-systems.com>

import os
import sys

THREAD_PRIO = {
    3: 6,
    4: 4,
    5: 5,
    6: 2,
    7: 4
}

LOCK_DEPTH = {
    3: 5,
    4: 3,
    5: 3,
    6: 4,
    7: 5
}


def thread_prio_sort(x):
    return THREAD_PRIO.get(x) * 1000 + x


def testfunc(child):
    for k in THREAD_PRIO:
        child.expect("T{} \(prio {}, depth 0\): trying to lock rmutex now"
                     .format(k, THREAD_PRIO[k]))

    pri_sorted = sorted(THREAD_PRIO, key=thread_prio_sort)
    for T in pri_sorted:
        for depth in range(LOCK_DEPTH[T]):
            child.expect("T{} \(prio {}, depth {}\): locked rmutex now"
                         .format(T, THREAD_PRIO[T], depth))

if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
