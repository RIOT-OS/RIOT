#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(child):
    child.expect_exact('micro-ecc compiled!')
    child.expect_exact('Testing 16 random private key pairs and signature '
                       'without using HWRNG')
    child.expect_exact('................ done with 0 error(s)')
    child.expect_exact('SUCCESS')

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc, timeout=60))
