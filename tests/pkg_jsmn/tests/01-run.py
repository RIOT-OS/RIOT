#!/usr/bin/env python3

import os
import sys


def testfunc(child):
    child.expect_exact('- User: johndoe')
    child.expect_exact('- Admin: false')
    child.expect_exact('- UID: 1000')
    child.expect_exact('- Groups:')
    child.expect_exact('  * users')
    child.expect_exact('  * wheel')
    child.expect_exact('  * audio')
    child.expect_exact('  * video')


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
