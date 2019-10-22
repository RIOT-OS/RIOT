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


def thread_prio_sort(x):
    return thread_prio.get(x)*1000 + x


def testfunc(child):
    for k in thread_prio.keys():
        child.expect(u"T%i \(prio %i, depth 0\): trying to lock rmutex now" %
                     (k, thread_prio[k]))

    pri_sorted = sorted(thread_prio, key=thread_prio_sort)
    for T in pri_sorted:
        for depth in range(lock_depth[T]):
            child.expect(u"T%i \(prio %i, depth %i\): locked rmutex now" %
                         (T, thread_prio[T], depth))


if __name__ == "__main__":
    sys.exit(run(testfunc))
