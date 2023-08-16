#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('If you can read this:')
    child.expect_exact('4294967295')
    child.expect_exact('-2147483648')
    child.expect_exact('FA')
    child.expect_exact('-2147483648')
    child.expect_exact('12345678')
    child.expect_exact('123456789ABCDEF0')
    child.expect_exact('18446744073709551615')
    child.expect_exact('-9223372036854775808')
    child.expect_exact('1.23450')
    child.expect_exact('Test successful.')


if __name__ == "__main__":
    sys.exit(run(testfunc))
