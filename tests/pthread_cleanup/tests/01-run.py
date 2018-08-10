#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect('START')
    for i in range(5):
        child.expect_exact('<SCOPE {}{}>'
                           .format(i + 1, ' /' if i == 4 else ''))

    child.expect_exact('Cleanup: <5>')
    child.expect_exact('</SCOPE 4>')
    child.expect_exact('</SCOPE 3>')
    for i in (3, 2, 1):
        child.expect_exact('Cleanup: <{}>'.format(i))
    child.expect_exact('Result: 1234')
    child.expect('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
