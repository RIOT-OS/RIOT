#!/usr/bin/env python3

import sys
from testrunner import run


# Use a custom global timeout for slow hardware. On microbit (nrf51), the
# test completes in 120s.
TIMEOUT = 200


def testfunc(child):
    child.expect_exact('micro-ecc compiled!')
    child.expect_exact('Testing 16 random private key pairs and signature '
                       'using HWRNG')
    child.expect_exact('................ done with 0 error(s)')
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=TIMEOUT))
