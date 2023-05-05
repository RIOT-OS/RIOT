#!/usr/bin/env python3

import sys
from testrunner import run


# Use a custom global timeout for slow hardware. On ATmegas clocked at 8MHz
# one test round completes in ~36s
TIMEOUT = 100


def testfunc(child):
    child.expect_exact('micro-ecc compiled!')
    child.expect(r'Testing (\d+) random private key pairs and signature '
                 'without using HWRNG')
    testrounds = int(child.match.group(1))
    for i in range(testrounds):
        child.expect_exact("Round {}".format(i))
    child.expect_exact('Done with 0 error(s)')
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=TIMEOUT))
