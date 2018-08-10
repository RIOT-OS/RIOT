#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('Executing test_get_addr()')
    child.expect_exact(' + succeeded.')
    child.expect_exact('Executing test_send()')
    child.expect_exact(' + succeeded.')
    child.expect_exact('Executing test_receive()')
    child.expect_exact(' + succeeded.')
    child.expect_exact('Executing test_set_addr()')
    child.expect_exact(' + succeeded.')
    child.expect_exact('ALL TESTS SUCCESSFUL')


if __name__ == "__main__":
    sys.exit(run(testfunc))
