#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("event 0")
    child.expect_exact("event A")
    child.expect_exact("event D")
    child.expect_exact("event B")
    child.expect_exact("event A")
    child.expect_exact("event C")
    child.expect_exact("event A")
    child.expect_exact("event B")
    child.expect_exact("event A")
    child.expect_exact("event A")
    child.expect_exact("event C")
    child.expect_exact("event B")
    child.expect_exact("event A")


if __name__ == "__main__":
    sys.exit(run(testfunc))
