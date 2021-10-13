#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('main done\r\n')
    child.expect_exact('high\r\n')
    child.expect_exact('medium\r\n')
    child.expect_exact('low\r\n')


if __name__ == "__main__":
    sys.exit(run(testfunc))
