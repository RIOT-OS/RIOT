#!/usr/bin/env python3

import os
import sys


def testfunc(child):
    child.expect('START')
    child.expect('SUCCESS')


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTTOOLS'], 'testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
