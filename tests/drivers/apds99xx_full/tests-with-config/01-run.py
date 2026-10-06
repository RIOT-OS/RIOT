#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Gunar Schorcht <gunar@schorcht.net>
# SPDX-License-Identifier: LGPL-2.1-only

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
