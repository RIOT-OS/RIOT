#!/usr/bin/env python3

import sys
import time
from testrunner import run


def testfunc(child):
    child.expect_exact(">")
    time.sleep(1)  # Wait 1s to let some time for the interface to be fully ready
    child.sendline("ifconfig")
    child.expect(r"Iface\s+(\d+)\s+HWaddr:")


if __name__ == "__main__":
    sys.exit(run(testfunc))
