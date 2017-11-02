#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(child):
    child.expect_exact(u'[START] Spawning threads')
    child.expect(r'\.+')
    child.expect(r'\[SUCCESS\] created \d+')

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
