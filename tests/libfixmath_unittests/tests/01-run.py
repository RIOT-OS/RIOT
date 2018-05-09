#!/usr/bin/env python3

# Copyright (C) 2017 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

# Float and print operations are slow on boards
# Got 80 iotlab-m3 and 250 on samr21-xpro
TIMEOUT = 300


def testfunc(child):
    child.expect('SUCCESS', timeout=TIMEOUT)


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
