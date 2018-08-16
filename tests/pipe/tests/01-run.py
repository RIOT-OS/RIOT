#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('Start.')
    child.expect_exact('Middle read: <ABCD> [0:4]')
    child.expect_exact('Middle read: <EF> [4:6]')
    child.expect_exact('End read: <ABC> [0:3]')
    child.expect_exact('End read: <DEF> [3:6]')
    child.expect_exact('Middle read: <GHIJ> [6:10]')
    child.expect_exact('Middle read: <KL> [10:12]')
    child.expect_exact('End read: <GHI> [6:9]')
    child.expect_exact('End read: <JKL> [9:12]')
    child.expect_exact('Middle read: <MNOP> [12:16]')
    child.expect_exact('Middle read: <QR> [16:18]')
    child.expect_exact('End read: <MNO> [12:15]')
    child.expect_exact('End read: <PQR> [15:18]')
    child.expect_exact('Middle read: <STUV> [18:22]')
    child.expect_exact('Middle read: <WX> [22:24]')
    child.expect_exact('Main done.')
    child.expect_exact('End read: <STU> [18:21]')
    child.expect_exact('End read: <VWX> [21:24]')
    child.expect_exact('Middle read: <YZ> [24:26]')
    child.expect_exact('Middle done.')
    child.expect_exact('End read: <YZ> [24:26]')
    child.expect_exact('End done.')


if __name__ == "__main__":
    sys.exit(run(testfunc))
