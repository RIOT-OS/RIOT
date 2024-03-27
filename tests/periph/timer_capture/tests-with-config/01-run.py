#!/usr/bin/env python3

# Copyright (C) 2024 Otto-von-Guericke-Universität Magdeburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
from testrunner import run


def testfunc(child):
    board = os.environ['BOARD']
    child.expect_exact(f'testing periph_timer_capture on {board}')
    child.expect_exact('TEST SUCCEEDED')


if __name__ == "__main__":
    sys.exit(run(testfunc))
