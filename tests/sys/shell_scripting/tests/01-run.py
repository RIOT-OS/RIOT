#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('Hello RIOT!')
    child.expect_exact('42')
    child.expect_exact('/nvm0/script.sh:6: error -7')


if __name__ == "__main__":
    sys.exit(run(testfunc))
