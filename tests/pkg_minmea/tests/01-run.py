#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('START')
    child.expect('parsed coordinates: lat=52.483631 lon=13.446008')
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
