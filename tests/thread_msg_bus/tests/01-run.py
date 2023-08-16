#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('THREAD 1 start')
    child.expect_exact('THREAD 2 start')
    child.expect_exact('THREAD 3 start')
    child.expect_exact('THREADS CREATED')

    child.expect_exact('Posted event 22 to 0 threads')

    child.expect_exact('T1 recv: Hello Threads! (type=23)')
    child.expect_exact('T3 recv: Hello Threads! (type=23)')
    child.expect_exact('Posted event 23 to 2 threads')

    child.expect_exact('T2 recv: Hello Threads! (type=24)')
    child.expect_exact('Posted event 24 to 1 threads')

    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
