#!/usr/bin/env python3

# Copyright (C) 2018 Acutam Automation, LLC
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect_exact("DAC extension test routine")
    child.expect_exact('Running DAC functions for internal device')
    child.expect('- Number of DAC channels: ')
    i = child.readline()
    for _ in range(int(i)):
        child.expect('- Init DAC channel \d+')
        child.expect('- Set DAC channel \d+: \d+')
        child.expect('- Poweroff DAC channel \d+')
        child.expect('- Poweron DAC channel \d+')
    child.expect_exact('Running DAC functions for extension test device')
    child.expect('test_dac_channels dev 0xbeef')
    child.expect('- Number of DAC channels: ')
    i = child.readline()
    for _ in range(int(4)):
        child.expect('- Init DAC channel \d+')
        child.expect('test_dac_init dev 0xbeef, chn \d+')
        child.expect('- Set DAC channel \d+: \d+')
        child.expect('test_dac_set dev 0xbeef, chn \d+, val \d+')
        child.expect('- Poweroff DAC channel \d+')
        child.expect('test_dac_poweroff dev 0xbeef, chn \d+')
        child.expect('- Poweron DAC channel \d+')
        child.expect('test_dac_poweron dev 0xbeef, chn \d+')
    child.expect_exact("Running DAC functions for extension notsup device")
    child.expect_exact("(they should not print output)")
    for _ in range(int(i)):
        child.expect('- notsup dac.h functions on channel \d+')
    child.expect_exact("Checking that all channels in range have init error using notsup")
    child.expect_exact("(lack of init error implies improper redirection)")
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
