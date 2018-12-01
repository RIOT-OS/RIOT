#!/usr/bin/env python3

# Copyright (C) 2018 Acutam Automation, LLC
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect_exact("ADC extension test routine")
    child.expect_exact('Running ADC functions for internal device')
    child.expect_exact('- Number of ADC channels: ')
    i = child.readline()
    for _ in range(int(i)):
        child.expect('- Init ADC channel \d+')
        child.expect('- Sample of ADC channel \d+: \d+')
    child.expect_exact('Running ADC functions for extension test device')
    child.expect_exact('test_adc_channels dev=0xbeef')
    child.expect_exact('- Number of ADC channels: ')
    c = child.readline('\d+')
    for _ in range(int(c)):
        child.expect('- Init ADC channel \d+')
        child.expect('test_adc_init dev=0xbeef chn=\d+')
        child.expect('test_adc_sample dev=0xbeef chn=\d+')
        child.expect('- Sample of ADC channel \d+: \d+')
    child.expect_exact("Running ADC functions for extension notsup device")
    child.expect_exact("(they should not print output)")
    for _ in range(4):
        child.expect('- notsup adc.h functions on channel \d+')
    child.expect_exact("Checking that all channels in range have init error using notsup")
    child.expect_exact("(lack of init error implies improper redirection)")
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
