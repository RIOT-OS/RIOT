#!/usr/bin/env python3

# Copyright (C) 2017 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run

# Float and print operations are slow on boards
# Got 80 iotlab-m3, 250 on samr21-xpro and 640 on microbit
TIMEOUT = 1000


def testfunc(child):
    child.expect('SUCCESS', timeout=TIMEOUT)


if __name__ == "__main__":
    sys.exit(run(testfunc))
