#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
from testrunner import run


# on real hardware, this test application can take several minutes to
# complete (~4min on microbit)
HW_TIMEOUT = 300


def testfunc(child):
    board = os.environ['BOARD']
    # Increase timeout on "real" hardware
    timeout = HW_TIMEOUT if board is not 'native' else -1
    child.expect('OK \(\d+ tests\)', timeout=timeout)


if __name__ == "__main__":
    sys.exit(run(testfunc))
