#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(child):
    child.expect("START")
    for i in range(5):
        child.expect_exact("+++ timeout  {} +++".format(i + 1))
    child.expect("SUCCESS")


if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
