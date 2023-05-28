#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("empty event source")
    child.expect_exact("occupied event source")
    child.expect_exact("event C")
    child.expect_exact("event B")
    child.expect_exact("event A")


if __name__ == "__main__":
    sys.exit(run(testfunc))
