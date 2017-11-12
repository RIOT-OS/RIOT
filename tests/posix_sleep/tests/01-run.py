#!/usr/bin/env python3

# Copyright (C) 2017 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def _check_usleep(child):
    child.expect_exact('usleep 1 x 1000*1000')
    for i in range(5):
        child.expect_exact('calling usleep({})'.format(i * 1000 * 1000))
        child.expect_exact('wake up', timeout=(i+1))


def _check_sleep(child):
    child.expect_exact('sleep 1')
    for i in range(5):
        child.expect_exact('calling sleep({})'.format(i))
        child.expect_exact('wake up', timeout=(i+1))


def testfunc(child):
    _check_usleep(child)
    _check_sleep(child)
    child.expect('SUCCESS')

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
