#!/usr/bin/env python3

# Copyright (C) 2023 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

from testrunner import run

EXPECTED_HELP = (
    'Command              Description',
    '---------------------------------------',
    'coreclk              Print the CPU frequency',
)
BOARD = os.environ['BOARD']


def testfunc(child):
    # avoid sending an extra empty line on native.
    if BOARD in ['native', 'native64']:
        child.crlf = '\n'

    # check the help
    child.sendline('help')
    for line in EXPECTED_HELP:
        child.expect_exact(line)

    # coreclk
    child.sendline('coreclk')
    child.expect(r"core clock: \d+ Hz")


if __name__ == "__main__":
    sys.exit(run(testfunc))
