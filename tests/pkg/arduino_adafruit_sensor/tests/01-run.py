#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2021 Gunar Schorchtr <gunar@schorcht.net>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('------------------------------------')
    child.expect_exact('Sensor:       ADXL345')
    child.expect_exact('Type:         Acceleration (m/s2)')
    child.expect_exact('Driver Ver:   1')
    child.expect_exact('Unique ID:    12345')
    child.expect_exact('Min Value:    -156.90')
    child.expect_exact('Max Value:    156.90')
    child.expect_exact('Resolution:   0.03')
    child.expect_exact('------------------------------------')


if __name__ == "__main__":
    sys.exit(run(testfunc))
