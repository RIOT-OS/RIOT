#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(child):
    child.expect_exact('If you can read this:')
    child.expect_exact('Test successful.')


if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
