#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('./main.c')
    child.expect_exact('./module/a.c')
    child.expect_exact('./module/b.c')
    child.expect_exact('./folder/a.c')
    child.expect_exact('./folder/subfolder/b.c')
    child.expect_exact('./folder/subfolder/c.c')


if __name__ == "__main__":
    sys.exit(run(testfunc))
