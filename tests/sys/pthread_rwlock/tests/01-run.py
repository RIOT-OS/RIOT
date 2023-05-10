#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect('START')
    for _ in range(8):
        child.expect('start')

    for _ in range(8):
        child.expect('done')

    child.expect('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
