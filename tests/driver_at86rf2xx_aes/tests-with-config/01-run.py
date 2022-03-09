#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect('START')
    child.expect('ECB test 1 done')
    child.expect('ECB test 2 done')
    child.expect('CBC test 1 done')
    child.expect('CBC test 2 done')
    child.expect('CBC test 3 done')
    child.expect('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
