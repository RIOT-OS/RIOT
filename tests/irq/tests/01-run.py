#!/usr/bin/env python3

import sys


def testfunc(child):
    child.expect('START')
    child.expect('SUCCESS')


if __name__ == "__main__":
    from testrunner import run
    sys.exit(run(testfunc))
