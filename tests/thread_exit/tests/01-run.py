#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(child):
    child.expect("main: starting")
    child.expect("main: exiting")
    child.expect("2nd: starting")
    child.expect("3rd: starting")
    child.expect("3rd: exiting")
    child.expect("2nd: exiting")
    child.expect("4th: starting")
    child.expect("4th: exiting")


if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
