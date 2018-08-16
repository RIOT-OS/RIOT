#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('micro-ecc compiled!')
    child.expect_exact('Testing 16 random private key pairs and signature '
                       'without using HWRNG')
    child.expect_exact('................ done with 0 error(s)')
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=60))
