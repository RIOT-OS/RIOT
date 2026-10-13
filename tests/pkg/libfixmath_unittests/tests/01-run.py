#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run

# Float and print operations are slow on boards
# Got 80 iotlab-m3, 250 on samr21-xpro and 640 on microbit
TIMEOUT = 1000


def testfunc(child):
    child.expect('SUCCESS', timeout=TIMEOUT)


if __name__ == "__main__":
    sys.exit(run(testfunc))
