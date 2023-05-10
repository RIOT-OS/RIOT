#!/usr/bin/env python3

import sys
from testrunner import run


# This test can take some time to complete when testing on hardware (e.g
# on samr21-xpro) and the default timeout (10s) is not enough.
# For example, it takes about 2 minutes to complete on a microbit.
TIMEOUT = 150


def testfunc(child):
    child.expect('START')
    child.expect('condition fulfilled.')
    child.expect('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=TIMEOUT))
