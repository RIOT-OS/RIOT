#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect([r"OK \([0-9]+ tests\)",
                  r"error: unable to initialize RTC \[I2C initialization error\]"])


if __name__ == "__main__":
    from testrunner import run
    sys.exit(run(testfunc))
