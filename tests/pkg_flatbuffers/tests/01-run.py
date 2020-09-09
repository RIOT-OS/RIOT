#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('The FlatBuffer was successfully created and verified!')


if __name__ == "__main__":
    sys.exit(run(testfunc))
