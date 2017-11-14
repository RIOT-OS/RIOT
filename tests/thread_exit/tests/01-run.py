#!/usr/bin/env python3

import os
import sys


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
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
