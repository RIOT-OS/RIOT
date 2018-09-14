#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect("START")
    child.expect_exact("thread(): waiting for 0x1...")
    child.expect_exact("main(): setting flag 0x0001")
    child.expect_exact("thread(): received flags: 0x0001")
    child.expect_exact("thread(): waiting for 0x1 || 0x64...")
    child.expect_exact("main(): setting flag 0x0064")
    child.expect_exact("thread(): received flags: 0x0064")
    child.expect_exact("thread(): waiting for 0x2 && 0x4...")
    child.expect_exact("main(): setting flag 0x0001")
    child.expect_exact("main(): setting flag 0x0008")
    child.expect_exact("main(): setting flag 0x0002")
    child.expect_exact("main(): setting flag 0x0004")
    child.expect_exact("thread(): received flags: 0x0006")
    child.expect_exact("thread(): waiting for any flag, one by one")
    child.expect_exact("thread(): received flags: 0x0001")
    child.expect_exact("thread(): waiting for any flag, one by one")
    child.expect_exact("thread(): received flags: 0x0008")
    child.expect_exact("main: setting 100ms timeout...")
    child.expect("main: timeout triggered. time passed: [0-9]{6}us")
    child.expect("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc))
