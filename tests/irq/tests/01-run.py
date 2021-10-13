#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect('START')
    child.expect('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
