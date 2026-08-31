#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('boot.py: MicroPython says hello!')
    child.expect_exact('>>>')

    # define a variable that should not survive a soft reset
    child.sendline('a = 42')
    child.expect_exact('>>>')

    # trigger a soft reset (ideally via CTRL + D, but native does not pass
    # control characters through to the REPL)
    child.sendline('raise SystemExit')
    child.expect_exact('soft reboot')
    child.expect_exact('>>>')

    # boot.py is only executed on cold boot, so it must not run again
    assert '-- Executing boot.py' not in child.before

    # the previous interpreter state, including the heap, must be gone
    child.sendline('print(a)')
    child.expect_exact('NameError')
    child.expect_exact('>>>')

    print("[TEST PASSED]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
