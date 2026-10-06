#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Gunar Schorcht <gunar@schorcht.net>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('Initialize BME680 sensor 0 ... OK')
    child.expect(r'\[bme680\]: dev=0, '
                 r'T = \d+.\d+ degC, '
                 r'P = \d+ Pa, '
                 r'H = \d+.\d+ \%, '
                 r'G = \d+ ohms\r\n')
    print('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
