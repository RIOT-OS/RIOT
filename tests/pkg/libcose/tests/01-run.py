#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import os
import sys
from testrunner import run_check_unittests
from testrunner import TIMEOUT as DEFAULT_TIMEOUT


BOARD = os.environ['BOARD']
# on real hardware, this test application can take several minutes to
# complete (>5min on nrf51dk)
TIMEOUT = 400 if BOARD not in ['native', 'native32', 'native64'] else DEFAULT_TIMEOUT


if __name__ == "__main__":
    sys.exit(run_check_unittests(timeout=TIMEOUT))
