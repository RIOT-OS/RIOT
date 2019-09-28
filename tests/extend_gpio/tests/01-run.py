#!/usr/bin/env python3

# Copyright (C) 2018 Acutam Automation, LLC
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect_exact("gpio extension test routine")
    for _ in range(4):
        child.expect('Running gpio.h functions on pin \d+')
        child.expect('init on dev 0xbeef with pin \d+')
        child.expect('init_int on dev 0xbeef with pin \d+')
        child.expect('irq_enable on dev 0xbeef with pin \d+')
        child.expect('irq_disable on dev 0xbeef with pin \d+')
        child.expect('read on dev 0xbeef with pin \d+')
        child.expect('set on dev 0xbeef with pin \d+')
        child.expect('clear on dev 0xbeef with pin \d+')
        child.expect('toggle on dev 0xbeef with pin \d+')
        child.expect('write on dev 0xbeef with pin \d+')
    child.expect_exact("Running notsup functions")
    child.expect_exact("(they should not print output)")
    for _ in range(4):
        child.expect('Running notsup gpio.h functions on pin \d+')
    child.expect_exact("Checking that all pins in range have init error using notsup")
    child.expect_exact("(lack of init error implies improper redirection)")
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
