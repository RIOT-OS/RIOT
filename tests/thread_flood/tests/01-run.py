#!/usr/bin/env python3

import os
import sys


def testfunc(child):
    child.expect_exact(u'[START] Spawning threads')
    child.expect(r'\.+')
    child.expect(r'\[SUCCESS\] created \d+')


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
