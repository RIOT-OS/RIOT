#!/usr/bin/env python3

import os
import sys


def testfunc(child):
    child.expect_exact('START')
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    from testrunner import run
    sys.exit(run(testfunc))
