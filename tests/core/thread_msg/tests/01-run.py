#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('THREADS CREATED')
    child.expect_exact('THREAD 1 start')
    child.expect_exact('THREAD 2 start')
    child.expect_exact('THREAD 3 start')
    child.expect_exact('THREAD 1 end')
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
