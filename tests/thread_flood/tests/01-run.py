#!/usr/bin/env python3

import sys


def testfunc(child):
    child.expect_exact(u'[START] Spawning threads')
    child.expect(r'\.+')
    child.expect(r'\[SUCCESS\] created \d+')


if __name__ == "__main__":
    from testrunner import run
    sys.exit(run(testfunc))
