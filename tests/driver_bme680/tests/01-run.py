#!/usr/bin/env python3

# Copyright (C) 2020 Gunar Schorcht <gunar@schorcht.net>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect('Initialize BME680 sensor 0 ... ')
    i = child.expect(['[OK]', '[failed]'])
    if i == 1:
        print('FAILED')
        return
    child.expect('\[bme680\]: dev=0, ')
    child.expect(r'T = \d+.\d+ degC, ')
    child.expect(r'P = \d+ Pa, ')
    child.expect(r'H = \d+.\d+ \%, ')
    child.expect(r'G = \d+ ohms')
    print('SUCCESS')
    return


if __name__ == "__main__":
    sys.exit(run(testfunc))
