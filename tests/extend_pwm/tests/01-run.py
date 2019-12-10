#!/usr/bin/env python3

# Copyright (C) 2018 Acutam Automation, LLC
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect_exact("PWM extension test routine")
    o = child.expect(['Running PWM functions for onboard devices', 'No PWM onboard devices'])
    if o == 0:
        child.expect_exact('Number of onboard devices: ')
        d = child.readline()
        for _ in range(int(d)):
            child.expect('- Init PWM device \d+')
            child.expect_exact('- Number of PWM channels: ')
            c = child.readline()
            for _ in range(int(c)):
                child.expect('- Set PWM channel \d+: \d+')
        child.expect_exact('- Poweroff PWM device')
        child.expect_exact('- Poweron PWM device')
    child.expect_exact('Running PWM functions for extension test device')
    child.expect('- Init PWM device \d+')
    child.expect('test_pwm_init dev 0xbeef, mode \d+, freq \d+, res \d+')
    child.expect_exact('test_pwm_channels dev 0xbeef')
    child.expect_exact('- Number of PWM channels: ')
    i = child.readline()
    for _ in range(int(i)):
        child.expect('- Set PWM channel \d+: \d+')
        child.expect('test_pwm_set dev 0xbeef, chn \d+, val \d+')
    child.expect_exact('- Poweroff PWM device')
    child.expect_exact('test_pwm_poweroff dev 0xbeef')
    child.expect_exact('- Poweron PWM device')
    child.expect_exact('test_pwm_poweron dev 0xbeef')
    child.expect_exact("Running PWM functions for extension notsup device")
    child.expect_exact("(they should not print output)")
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
