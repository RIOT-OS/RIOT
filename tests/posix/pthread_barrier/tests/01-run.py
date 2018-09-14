#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect('START')
    for i in range(4):
        child.expect('Start {}'.format(i + 1))
    child.expect('Done 2')
    child.expect('Done 1')
    child.expect('Done 3')
    child.expect('Done 4')
    child.expect('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
