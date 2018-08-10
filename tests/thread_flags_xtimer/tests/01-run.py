#!/usr/bin/env python3

import os
import sys


def testfunc(child):
    child.expect("START")
    for i in range(5):
        child.expect_exact("+++ timeout  {} +++".format(i + 1))
    child.expect("SUCCESS")


if __name__ == "__main__":
    from testrunner import run
    sys.exit(run(testfunc))
