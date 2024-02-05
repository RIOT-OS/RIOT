#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
from testrunner import run_check_unittests
from testrunner import TIMEOUT as DEFAULT_TIMEOUT


BOARD = os.environ['BOARD']
# on real hardware, this test application can take several minutes to
# complete (>5min on nrf51dk)
TIMEOUT = 400 if BOARD not in ['native', 'native64'] else DEFAULT_TIMEOUT


if __name__ == "__main__":
    sys.exit(run_check_unittests(timeout=TIMEOUT))
