#!/usr/bin/env python3

#  Copyright (C) 2019 Freie Universitaet Berlin,
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>

import sys
import pexpect
from testrunner import run


def testfunc(child):
    # Try to wait for the shell
    for _ in range(0, 10):
        child.sendline("help")
        if child.expect_exact(["> ", pexpect.TIMEOUT], timeout=1) == 0:
            break
    child.sendline("mutex_timeout_test")
    child.expect("starting test: xtimer mutex lock timeout")
    child.expect("OK")
    child.expect_exact("> ")


if __name__ == "__main__":
    sys.exit(run(testfunc))
