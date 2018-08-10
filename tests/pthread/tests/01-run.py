#!/usr/bin/env python3

import os
import sys
import math

FACTORIAL_PARAM = 6


def testfunc(child):
    child.expect('main: parameter = {}'.format(FACTORIAL_PARAM))
    child.expect('pthread: parameter = {}'.format(FACTORIAL_PARAM))
    child.expect('pthread: factorial = {}'
                 .format(math.factorial(FACTORIAL_PARAM)))
    child.expect('main: factorial = {}'
                 .format(math.factorial(FACTORIAL_PARAM)))
    child.expect('SUCCESS')


if __name__ == "__main__":
    from testrunner import run
    sys.exit(run(testfunc))
