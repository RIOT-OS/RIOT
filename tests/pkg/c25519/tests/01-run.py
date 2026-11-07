#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-FileCopyrightText: 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
# SPDX-License-Identifier: LGPL-2.1-only

import os
import sys

from testrunner import run_check_unittests
from testrunner import TIMEOUT as DEFAULT_TIMEOUT


BOARD = os.environ['BOARD']
# Increase timeout on "real" hardware
# 170 seconds on `arduino-mega2560`
# ~300 seconds on `z1`
TIMEOUT = 320 if BOARD not in ['native', 'native32', 'native64'] else DEFAULT_TIMEOUT


if __name__ == "__main__":
    sys.exit(run_check_unittests(timeout=TIMEOUT))
