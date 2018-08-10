#!/usr/bin/env python3

import os
import sys

NB_TESTS = 3


def testfunc(child):
    for test in range(NB_TESTS):
        child.expect_exact('Test {}:'.format(test + 1))
        child.expect('Result: [0-9A-Z]+ SUCCESS')


if __name__ == "__main__":
    from testrunner import run
    sys.exit(run(testfunc))
