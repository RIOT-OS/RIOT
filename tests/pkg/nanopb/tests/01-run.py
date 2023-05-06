#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('Your lucky number was 13!')


if __name__ == "__main__":
    sys.exit(run(testfunc))
