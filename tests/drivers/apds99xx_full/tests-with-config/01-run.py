#!/usr/bin/env python3

# Copyright (C) 2020 Gunar Schorcht <gunar@schorcht.net>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('Initializing APDS99XX sensor')
    child.expect_exact('[OK]')
    child.expect(r'ambient = \d+ \[cnts\]')
    child.expect([r'red = \d+ \[cnts\], green = \d+ \[cnts\], blue = \d+ \[cnts\]',
                  r'illuminance = %d [lux]'])
    print('SUCCESS')
    return


if __name__ == "__main__":
    sys.exit(run(testfunc))
