#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('If you can read this:')
    for i in range(0, 255):
        child.expect_exact(str(i))
    child.expect_exact('Test successful.')


if __name__ == "__main__":
    sys.exit(run(testfunc))
