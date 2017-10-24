#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(term):
    term.expect('Start spawning\r\n')
    term.expect('Thread creation successful aborted\r\n')


if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc, echo=True))
