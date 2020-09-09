#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Predicted label: 7")


if __name__ == "__main__":
    sys.exit(run(testfunc))
