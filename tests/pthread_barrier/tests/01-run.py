#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect(r'NUM_CHILDREN: (\d+), NUM_ITERATIONS: (\d+)\r\n')
    children = int(child.match.group(1))
    iterations = int(child.match.group(2))
    for i in range(children):
        child.expect('Start {}'.format(i + 1))
    for _ in range(iterations):
        sleeps = []
        for _ in range(children):
            child.expect(r'Child (\d+) sleeps for \s* (\d+) us.\r\n')
            child_num = int(child.match.group(1))
            sleep = int(child.match.group(2))
            sleeps.append([sleep, child_num])
    for _, child_num in sorted(sleeps):
        child.expect(r'Done (\d+)\r\n')
        assert(child_num == int(child.match.group(1)))
    child.expect('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
