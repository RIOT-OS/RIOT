#!/usr/bin/env python3

# Copyright (C) 2017 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('\tmember, sizeof, offsetof')
    ret = child.expect([r'sizeof\(thread_t\): [36, 48]',
                        r'sizeof\(thread_t\): [20, 26]'])
    if ret == 0:
        child.expect_exact('\tsp            4   0')
        child.expect_exact('\tstatus        1   4')
        child.expect_exact('\tpriority      1   5')
        child.expect_exact('\tpid           2   6')
        child.expect_exact('\trq_entry      4   8')
        child.expect_exact('\twait_data     4  12')
        child.expect_exact('\tmsg_waiters   4  16')
        child.expect_exact('\tmsg_queue    12  20')
        child.expect_exact('\tmsg_array     4  32')
    else:
        # 16 bit platform (wsn430)
        child.expect_exact('\tsp            2   0')
        child.expect_exact('\tstatus        1   2')
        child.expect_exact('\tpriority      1   3')
        child.expect_exact('\tpid           2   4')
        child.expect_exact('\trq_entry      2   6')
        child.expect_exact('\twait_data     2   8')
        child.expect_exact('\tmsg_waiters   2  10')
        child.expect_exact('\tmsg_queue     6  12')
        child.expect_exact('\tmsg_array     2  18')
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
