#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect('sender_thread start\r\n')
    child.expect('main thread alive\r\n')


if __name__ == "__main__":
    sys.exit(run(testfunc))
