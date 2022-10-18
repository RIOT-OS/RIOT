#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('THREAD 1 start')
    child.expect_exact('THREAD 2 start')
    child.expect_exact('THREAD 3 start')
    child.expect_exact('THREADS CREATED')

    child.expect_exact('Posted event 22 to 0 threads')

    child.expect_exact('T1 recv: Hello Threads! (type=23) from bus')
    child.expect_exact('T3 recv: Hello Threads! (type=23) from bus')
    child.expect_exact('Posted event 23 to 2 threads')

    child.expect_exact('T1 recv: Hello Threads! (type=24) from bus')
    child.expect_exact('T2 recv: Hello Threads! (type=24) from bus')
    child.expect_exact('Posted event 24 to 2 threads')

    child.expect_exact('Post message to thread 1')
    child.expect_exact('T1 recv: Hello Thread 1 (type=4919)')

    child.expect_exact('Post shutdown request to all threads')
    child.expect_exact('T1 recv: shutdown request (type=0) from bus')
    child.expect_exact('T2 recv: shutdown request (type=0) from bus')
    child.expect_exact('T3 recv: shutdown request (type=0) from bus')

    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
