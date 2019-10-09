#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.sendline("ifconfig")
    child.expect(r"Iface\s+(\d+)\s+HWaddr:")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1, echo=False))
