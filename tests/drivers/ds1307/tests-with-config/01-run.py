#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect([r"OK \([0-9]+ tests\)",
                  r"error: unable to initialize RTC \[I2C initialization error\]"])


if __name__ == "__main__":
    sys.exit(run(testfunc))
