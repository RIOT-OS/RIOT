#!/usr/bin/env python3

# Copyright (C) 2019 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import os
from testrunner import run


def testfunc(child):
    for _ in range(5):
        child.expect_exact('Logging value %d and string %s')

    if os.environ.get('BOARD', 'native') == 'native':
        child.expect_exact('native: exiting')


if __name__ == "__main__":
    sys.exit(run(testfunc))
