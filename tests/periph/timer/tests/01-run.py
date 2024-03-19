#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    # Make sure the expected application is actually flashed
    child.expect('Test for peripheral TIMERs')
    # The C application carefully evaluates the test results, no need to
    # re-implement that wheel in python and just check for the test to succeed
    child.expect('TEST SUCCEEDED')


if __name__ == "__main__":
    sys.exit(run(testfunc))
