#!/usr/bin/env python3

import os
import sys


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
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
