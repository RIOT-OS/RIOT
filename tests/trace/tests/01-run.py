#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"n=   0 t=\ +\d+ v=0x00000000\r\n")
    child.expect(r"n=   1 t=\+\ +\d+ v=0x00000001\r\n")


if __name__ == "__main__":
    sys.exit(run(testfunc))
