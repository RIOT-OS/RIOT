#!/usr/bin/env python3

import os
import sys


def testfunc(child):
    child.expect('START')
    child.expect('condition fulfilled.')
    child.expect('SUCCESS')


if __name__ == "__main__":
    # This test can take some time to complete when testing on hardware (e.g
    # on samr21-xpro) and the default timeout (10s) is not enough.
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc, timeout=60))
