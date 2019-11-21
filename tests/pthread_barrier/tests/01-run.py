#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect(r'NUM_CHILDREN: (\d+), NUM_ITERATIONS: (\d+)')
    children = int(child.match.group(1))
    iterations = int(child.match.group(2))
    for i in range(children):
        child.expect('Start {}'.format(i + 1))
    for _ in range(children * iterations):
        child.expect(r'Child \d sleeps for [" "\d]+ us.')
    child.expect('Done 2')
    child.expect('Done 1')
    child.expect('Done 3')
    child.expect('Done 4')
    child.expect('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
