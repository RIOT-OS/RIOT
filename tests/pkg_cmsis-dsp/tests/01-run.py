#!/usr/bin/env python3

import os
import sys


def testfunc(child):
    child.expect(r"dsp:   0\.903941\d{3}")
    child.expect_exact("reference:   0.903941793931839")
    child.expect(r"diff:   0.000000[01]\d{2}")


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTTOOLS'], 'testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
