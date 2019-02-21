#!/usr/bin/env python3

# Copyright (C) 2018 Federico Pellegrin <fede@evolware.org>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    # writes and verifies the last page of the flash
    child.sendline("test_last")
    child.expect_exact('wrote local page buffer to last flash page')
    child.expect('>')

    # check if board has raw write capability and if so test that as well
    # capability is deduced from help contents
    child.sendline("help")
    index = child.expect(['test_last_raw', '>'])
    if index == 0:
        child.sendline("test_last_raw")
        child.expect_exact('wrote raw short buffer to last flash page')
        child.expect('>')


if __name__ == "__main__":
    sys.exit(run(testfunc))
