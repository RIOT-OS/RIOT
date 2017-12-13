#!/usr/bin/env python3

import os
import sys


def testfunc(child):
    child.expect_exact('THREADS CREATED')
    child.expect_exact('THREAD 1 start')
    child.expect_exact('THREAD 2 start')
    child.expect_exact('THREAD 3 start')
    child.expect_exact('THREAD 1 end')
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
