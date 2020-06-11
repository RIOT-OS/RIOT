#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('__FILE__: ./tests/build_system_relpath/main.c\r\n')


if __name__ == "__main__":
    sys.exit(run(testfunc))
