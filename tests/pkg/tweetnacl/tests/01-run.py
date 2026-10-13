#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import os
import sys
from testrunner import run, check_unittests


def testfunc(child):
    board = os.environ['BOARD']
    # Increase timeout on "real" hardware
    timeout = 120 if board not in ['native', 'native32', 'native64'] else -1
    check_unittests(child, timeout=timeout)


if __name__ == "__main__":
    sys.exit(run(testfunc))
