#!/usr/bin/env python3

import sys
import math
from testrunner import run

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
    sys.exit(run(testfunc))
