#!/usr/bin/env python3

import sys
from testrunner import run

NB_TESTS = 2


def testfunc(child):
    for test in range(NB_TESTS):
        child.expect_exact('Test {}'.format(test + 1))
        child.expect('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
