#!/usr/bin/env python3

import sys


def testfunc(child):
    child.expect_exact('SUCCESS: Libcoap compiled!')


if __name__ == "__main__":
    from testrunner import run
    sys.exit(run(testfunc))
