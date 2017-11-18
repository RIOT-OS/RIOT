#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(child):
    child.expect('START')
    for _ in range(8):
        child.expect('start')

    for _ in range(8):
        child.expect('done')

    child.expect('SUCCESS')


if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
