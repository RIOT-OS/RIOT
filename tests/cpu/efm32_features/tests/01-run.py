#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Bas Stottelaar <basstottelaar@gmail.com>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect(
        r'Board booted, with some EFM32 features enabled or disabled.')


if __name__ == "__main__":
    sys.exit(run(testfunc))
