#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-License-Identifier: LGPL-2.1-only

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
