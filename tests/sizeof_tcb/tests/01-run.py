#!/usr/bin/env python3

# Copyright (C) 2017 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect_exact('\tmember, sizeof, offsetof')
    child.expect_exact('sizeof(thread_t): 36')
    child.expect_exact('\tsp            4   0')
    child.expect_exact('\tstatus        1   4')
    child.expect_exact('\tpriority      1   5')
    child.expect_exact('\tpid           2   6')
    child.expect_exact('\trq_entry      4   8')
    child.expect_exact('\twait_data     4  12')
    child.expect_exact('\tmsg_waiters   4  16')
    child.expect_exact('\tmsg_queue    12  20')
    child.expect_exact('\tmsg_array     4  32')
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
