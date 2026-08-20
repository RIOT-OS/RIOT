#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.sendline('import xtimer')
    child.expect_exact('>>>')

    child.sendline('import time')
    child.expect_exact('>>>')

    child.sendline('timer = xtimer.xtimer(lambda: print("Fired!"))')
    child.expect_exact('>>>')

    child.sendline('timer.set(5000000)')
    child.expect_exact('>>>')

    child.sendline('time.sleep(10)')
    child.expect_exact('>>>')

    child.sendline('print("Done")')

    # the timer fires first, then the time.sleep() finishes.
    child.expect_exact('Fired!', timeout=5)
    child.expect_exact('Done', timeout=10)

    child.expect_exact('>>>')

    print("[TEST PASSED]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
