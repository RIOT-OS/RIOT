#!/usr/bin/env python3

import os
import sys


def testfunc(child):
    child.expect_exact('03670110056700FF')
    child.expect_exact('0167FFD8067104D1FB2F0000')
    child.expect_exact('018806765EF2960A0003E8')


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTTOOLS'], 'testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
