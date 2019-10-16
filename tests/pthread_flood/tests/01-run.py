#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact(u'[START] Spawning threads')
    child.expect(r'\.+')
    child.expect("[SUCCESS]")
    child.expect("test end")


if __name__ == "__main__":
    sys.exit(run(testfunc))
