#!/usr/bin/env python3

import os
import sys


def testfunc(child):
    child.expect('START')
    for _ in range(8):
        child.expect('start')

    for _ in range(8):
        child.expect('done')

    child.expect('SUCCESS')


if __name__ == "__main__":
    from testrunner import run
    sys.exit(run(testfunc))
