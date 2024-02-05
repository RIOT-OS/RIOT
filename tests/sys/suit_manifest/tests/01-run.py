#!/usr/bin/env python3

# Copyright (C) 2018 Francisco Acosta <francisco.acosta@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
from testrunner import run


def testfunc(child):
    board = os.environ['BOARD']
    # Increase timeout on "real" hardware
    # 16 seconds on `samr21-xpro`
    # >50 seconds on `nrf51dk`
    timeout = 60 if board not in ['native', 'native64'] else -1
    child.expect(r"OK \(\d+ tests\)", timeout=timeout)


if __name__ == "__main__":
    sys.exit(run(testfunc))
