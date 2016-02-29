#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import pexpect

def init():
    if sys.version >= (3,):
        term = pexpect.spawnu("make term", timeout=0.5)
    else:
        term = pexpect.spawn("make term", timeout=0.5)
    term.logfile = sys.stdout
    return term

def test1(term):
    term.expect_exact('2: test_put1: mbox_put() => mbox_get() passed')
    term.expect_exact('3: test_get1: mbox_put() => mbox_get() passed')

def test2(term):
    term.expect_exact('2: test_put2: destroy message box')
    term.expect_exact('3: test_get2: mbox_get() message box was destroyed')

def test3(term):
    term.expect_exact('2: create new message box')
    term.expect_exact('2: test_put3: mbox_put() (i == 0) => mbox_get() passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 1) => mbox_get() passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 2) => mbox_get() passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 3) => mbox_get() passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 4) => mbox_get() passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 5) => mbox_get() passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 6) => mbox_get() passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 7) => mbox_get() passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 8) => mbox_get() passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 0) passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 9) => mbox_get() passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 1) passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 10) => mbox_get() passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 2) passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 11) => mbox_get() passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 3) passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 12) => mbox_get() passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 4) passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 13) => mbox_get() passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 5) passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 14) => mbox_get() passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 6) passed')
    term.expect_exact('2: test_put3: mbox_put() (i == 15) => mbox_get() passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 7) passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 8) passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 9) passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 10) passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 11) passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 12) passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 13) passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 14) passed')
    term.expect_exact('3: test_get3: mbox_put() => mbox_get() (i == 15) passed')
    term.expect_exact('3: test_get3: mbox is empty as expected')

def test4(term):
    term.expect_exact('2: test_put4: mbox_tryput() (i == 0) => mbox_get() passed')
    term.expect_exact('2: test_put4: mbox_tryput() (i == 1) => mbox_get() passed')
    term.expect_exact('2: test_put4: mbox_tryput() (i == 2) => mbox_get() passed')
    term.expect_exact('2: test_put4: mbox_tryput() (i == 3) => mbox_get() passed')
    term.expect_exact('2: test_put4: mbox_tryput() (i == 4) => mbox_get() passed')
    term.expect_exact('2: test_put4: mbox_tryput() (i == 5) => mbox_get() passed')
    term.expect_exact('2: test_put4: mbox_tryput() (i == 6) => mbox_get() passed')
    term.expect_exact('2: test_put4: mbox_tryput() (i == 7) => mbox_get() passed')
    term.expect_exact('2: test_put4: mbox_tryput() (i == 8) failed with EAGAIN as expected')
    term.expect_exact('3: test_get4: mbox_tryput() => mbox_get() (i == 0) passed')
    term.expect_exact('3: test_get4: mbox_tryput() => mbox_get() (i == 1) passed')
    term.expect_exact('3: test_get4: mbox_tryput() => mbox_get() (i == 2) passed')
    term.expect_exact('3: test_get4: mbox_tryput() => mbox_get() (i == 3) passed')
    term.expect_exact('3: test_get4: mbox_tryput() => mbox_get() (i == 4) passed')
    term.expect_exact('3: test_get4: mbox_tryput() => mbox_get() (i == 5) passed')
    term.expect_exact('3: test_get4: mbox_tryput() => mbox_get() (i == 6) passed')
    term.expect_exact('3: test_get4: mbox_tryput() => mbox_get() (i == 7) passed')
    term.expect_exact('3: test_get4: mbox is empty as expected')

def test5(term):
    term.expect_exact('2: test_put5: mbox_put() => mbox_tryget() passed')
    term.expect_exact('3: test_get5: mbox_put() => mbox_tryget() passed')
    term.expect_exact('3: test_get5: second mbox_tryget() failed as expected')

def test6(term):
    term.expect_exact('2: test_put6: mbox_put() => mbox_get_timed() passed')
    term.expect_exact('3: test_get6: mbox_put() => mbox_get_timed() passed')

def test7(term):
    term.expect_exact('2: test_put7: waiting for getter to time out')
    term.expect_exact('3: test_get7: mbox_put() => mbox_get_timed() timed out')


if __name__ == "__main__":
    TERM = init()
    test1(TERM)
    test2(TERM)
    test3(TERM)
    test4(TERM)
    TERM.expect_exact('All tests successful')
