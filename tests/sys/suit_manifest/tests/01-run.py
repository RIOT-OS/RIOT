#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2018 Francisco Acosta <francisco.acosta@inria.fr>
# SPDX-License-Identifier: LGPL-2.1-only

import os
import sys
from testrunner import run_check_unittests


if __name__ == "__main__":
    board = os.environ['BOARD']
    # Increase timeout on "real" hardware
    # 16 seconds on `samr21-xpro`
    # >50 seconds on `nrf51dk`
    timeout = 60 if board not in ['native', 'native32', 'native64'] else 10
    sys.exit(run_check_unittests(timeout=timeout))
