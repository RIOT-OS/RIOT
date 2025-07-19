#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("log_unit: Hello!")
    child.expect_exact("log_unit: Hello world!")

    # no unit name - match the beginning of the line
    child.expect("\n")
    child.expect("^Hello!")
    child.expect("\n")
    child.expect("^Hello world!")


if __name__ == "__main__":
    sys.exit(run(testfunc))
