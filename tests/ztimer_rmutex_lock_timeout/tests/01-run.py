#!/usr/bin/env python3

#  Copyright (C) 2020 Freie Universität Berlin,
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
    child.sendline("t1")
    child.expect("OK")
    child.expect_exact("> ")
    child.sendline("t2")
    child.expect("OK")
    child.expect_exact("> ")
    child.sendline("t3")
    child.expect("OK")
    child.expect_exact("> ")
    child.sendline("t4")
    child.expect("OK")
    child.expect_exact("> ")
    child.sendline("t5")
    child.expect("OK")
    child.expect_exact("> ")


if __name__ == "__main__":
    sys.exit(run(testfunc))
