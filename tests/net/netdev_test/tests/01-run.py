#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-License-Identifier: LGPL-2.1-only

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
