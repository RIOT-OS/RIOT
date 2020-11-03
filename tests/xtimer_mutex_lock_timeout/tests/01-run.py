#!/usr/bin/env python3

#  Copyright (C) 2019 Freie Universität Berlin,
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
    child.sendline("mutex_timeout_long_unlocked")
    child.expect_exact("starting test: xtimer mutex lock timeout")
    child.expect_exact("OK")
    child.expect_exact("> ")
    child.sendline("mutex_timeout_long_locked")
    child.expect_exact("starting test: xtimer mutex lock timeout")
    child.expect_exact("OK")
    child.expect_exact("> ")
    child.sendline("mutex_timeout_long_locked_low")
    child.expect_exact("starting test: xtimer mutex lock timeout with thread")
    child.expect(r"threads = (\d+)\r\n")
    num_threads = int(child.match.group(1))
    child.expect_exact("THREAD low prio: start")
    child.expect_exact("MAIN THREAD: calling xtimer_mutex_lock_timeout")
    child.expect_exact("OK")
    child.expect(r"threads = (\d+)\r\n")
    assert int(child.match.group(1)) == num_threads + 1
    child.expect_exact("MAIN THREAD: waiting for created thread to end")
    child.expect_exact("THREAD low prio: exiting low")
    child.expect(r"threads = (\d+)\r\n")
    assert int(child.match.group(1)) == num_threads
    child.expect_exact("> ")
    child.sendline("mutex_timeout_short_locked")
    child.expect_exact("starting test: xtimer mutex lock timeout with short timeout and locked mutex")
    child.expect_exact("OK")
    child.expect_exact("> ")
    child.sendline("mutex_timeout_short_unlocked")
    child.expect_exact("starting test: xtimer mutex lock timeout with short timeout and unlocked mutex")
    child.expect_exact("OK")
    child.expect_exact("> ")


if __name__ == "__main__":
    sys.exit(run(testfunc))
