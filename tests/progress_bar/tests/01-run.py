#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
from testrunner import run


TIMEOUT = 60
LENGTH = int(os.getenv('CONFIG_PROGRESS_BAR_LENGTH'))
FULL_CHARACTER = os.getenv('CONFIG_PROGRESS_BAR_FULL_CHARACTER')[1:-1]
EMPTY_CHARACTER = os.getenv('CONFIG_PROGRESS_BAR_EMPTY_CHARACTER')[1:-1]


def testfunc(child):
    for i in range(0, 100, 10):
        ratio = int(i * LENGTH / 100.0)
        progress_str = FULL_CHARACTER * ratio
        progress_str += EMPTY_CHARACTER * (LENGTH - ratio)
        check_str = 'Progress bar 0 |{}| {:3}%'.format(
            progress_str, i)
        child.expect_exact(check_str)
    child.expect_exact("Done!")

    for i in range(2, 6):  # 5 parallel progress bars
        check_str = 'Progress bar {} |{}|'.format(
            i, LENGTH * FULL_CHARACTER)
        child.expect_exact(check_str, timeout=TIMEOUT)
    child.expect_exact('Done!')


if __name__ == "__main__":
    sys.exit(run(testfunc))
