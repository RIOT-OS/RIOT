#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"dsp:   0\.903941\d{3}")
    child.expect_exact("reference:   0.903941793931839")
    child.expect(r"diff:   0.000000[01]\d{2}")


if __name__ == "__main__":
    sys.exit(run(testfunc))
