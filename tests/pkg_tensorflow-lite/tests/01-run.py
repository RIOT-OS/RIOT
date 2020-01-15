#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    # The default image of the test application contains a 7 (e.g. it's the
    # first image in the MNIST test dataset)
    child.expect_exact("Digit prediction: 7")


if __name__ == "__main__":
    sys.exit(run(testfunc))
