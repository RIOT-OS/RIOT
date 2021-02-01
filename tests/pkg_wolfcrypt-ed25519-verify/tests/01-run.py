#!/usr/bin/env python3

import sys
from testrunner import run


# This test needs some time to complete on small platforms. On nrf51dk, it
# takes >10s.
TIMEOUT = 20


def testfunc(child):
    child.expect_exact("The signature is valid!")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=TIMEOUT))
