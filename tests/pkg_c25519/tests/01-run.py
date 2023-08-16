#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

from testrunner import run_check_unittests
from testrunner import TIMEOUT as DEFAULT_TIMEOUT


BOARD = os.environ['BOARD']
# Increase timeout on "real" hardware
# 170 seconds on `arduino-mega2560`
# ~300 seconds on `z1`
TIMEOUT = 320 if BOARD != 'native' else DEFAULT_TIMEOUT


if __name__ == "__main__":
    sys.exit(run_check_unittests(timeout=TIMEOUT))
