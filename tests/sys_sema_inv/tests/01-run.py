#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('counter mode')
    child.expect_exact('THREAD 1 start')
    child.expect_exact('THREAD 2 start')
    child.expect_exact('THREAD 3 start')
    child.expect_exact('thread synced')
    child.expect_exact('THREAD 3 woke main thread')

    child.expect_exact('mask mode')
    child.expect_exact('THREAD 1 start')
    child.expect_exact('THREAD 2 start')
    child.expect_exact('THREAD 3 start')
    child.expect_exact('thread synced')
    child.expect_exact('THREAD 3 woke main thread')

    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
