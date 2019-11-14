#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Oliver Hahm <oliver.hahm@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

from testrunner import test_utils_interactive_sync

thread_prio = {
        3:  6,
        4:  4,
        5:  0,
        6:  2,
        7:  1
        }
first_group_size = 3


def testfunc(child):
    test_utils_interactive_sync(child)

    for k in thread_prio.keys():
        child.expect_exact("T{} (prio {}): waiting on condition variable now"
                           .format(k, thread_prio[k]))
    count = 0
    last = -1
    child.expect_exact("First batch was signaled")
    for _ in range(len(thread_prio)):
        child.expect(r"T\d+ \(prio (\d+)\): condition variable was signaled now")
        assert(int(child.match.group(1)) > last)
        last = int(child.match.group(1))
        count += 1
        if count == 3:
            child.expect_exact("First batch has woken up")
            child.expect_exact("Second batch was signaled")
    child.expect_exact("Second batch has woken up")


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
