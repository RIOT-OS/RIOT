#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#               2020 Gunar Schorcht <gunar@schorcht.net>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


# On slow platforms, like AVR, this test can take some time to complete.
TIMEOUT = 30


def testfunc(child):
    child.expect_exact(">")

    # execute bench for a MCU port to verify that MCU ports still work when
    # GPIO extension is enabled
    child.sendline("bench 0 0")
    child.expect(r" *nop loop: +(\d+)us  --- +(\d+\.\d+)us per call  --- +(\d+) calls per sec")
    child.expect(r" *gpio_set: +(\d+)us  --- +(\d+\.\d+)us per call  --- +(\d+) calls per sec")
    child.expect(r" *gpio_clear: +(\d+)us  --- +(\d+\.\d+)us per call  --- +(\d+) calls per sec")
    child.expect(r" *gpio_toggle: +(\d+)us  --- +(\d+\.\d+)us per call  --- +(\d+) calls per sec")
    child.expect(r" *gpio_read: +(\d+)us  --- +(\d+\.\d+)us per call  --- +(\d+) calls per sec")
    child.expect(r" *gpio_write: +(\d+)us  --- +(\d+\.\d+)us per call  --- +(\d+) calls per sec")
    child.expect_exact(" --- DONE ---")
    child.expect_exact(">")

    # test all functions with first example GPIO extender
    child.sendline("init_out exp0 3")
    child.expect(r"init dev (0[xX])?[\dA-Fa-f]+ \(foo1\) pin 3")
    child.expect_exact(">")
    child.sendline("init_out exp0 5")
    child.expect(r"init dev (0[xX])?[\dA-Fa-f]+ \(foo1\) pin 5")
    child.expect_exact(">")
    child.sendline("set exp0 3")
    child.expect(r"set dev (0[xX])?[\dA-Fa-f]+ \(foo1\) pins 0x08, state 0x08")
    child.expect_exact(">")
    child.sendline("read exp0 3")
    child.expect(r"read dev (0[xX])?[\dA-Fa-f]+ \(foo1\) state 0x08")
    child.expect(r"GPIO_PIN\(\d+, 3\) is HIGH")
    child.expect_exact(">")
    child.sendline("read exp0 5")
    child.expect(r"read dev (0[xX])?[\dA-Fa-f]+ \(foo1\) state 0x08")
    child.expect(r"GPIO_PIN\(\d+, 5\) is LOW")
    child.expect_exact(">")
    child.sendline("toggle exp0 5")
    child.expect(r"toggle dev (0[xX])?[\dA-Fa-f]+ \(foo1\) pins 0x20, state 0x28")
    child.expect_exact(">")
    child.sendline("read exp0 5")
    child.expect(r"read dev (0[xX])?[\dA-Fa-f]+ \(foo1\) state 0x28")
    child.expect(r"GPIO_PIN\(\d+, 5\) is HIGH")
    child.expect_exact(">")
    child.sendline("write exp0 5 0")
    child.expect(r"clear dev (0[xX])?[\dA-Fa-f]+ \(foo1\) pins 0x20, state 0x08")
    child.expect_exact(">")
    child.sendline("write exp0 5 1")
    child.expect(r"set dev (0[xX])?[\dA-Fa-f]+ \(foo1\) pins 0x20, state 0x28")
    child.expect_exact(">")

    # test the correct redirection to the other example GPIO expanders
    child.sendline("init_out exp1 1")
    child.expect(r"init dev (0[xX])?[\dA-Fa-f]+ \(foo2\) pin 1")
    child.expect_exact(">")
    child.sendline("init_out exp2 2")
    child.expect(r"init dev (0[xX])?[\dA-Fa-f]+ \(bar\) port 0, pin 2")
    child.expect_exact(">")
    child.sendline("init_out exp3 3")
    child.expect(r"init dev (0[xX])?[\dA-Fa-f]+ \(bar\) port 1, pin 3")
    child.expect_exact(">")
    child.sendline("init_out exp4 4")
    child.expect(r"init dev (0[xX])?[\dA-Fa-f]+ \(bar\) port 2, pin 4")
    child.expect_exact(">")
    child.sendline("init_out exp5 5")
    child.expect(r"init dev (0[xX])?[\dA-Fa-f]+ \(bar\) port 3, pin 5")
    child.expect_exact(">")

    print("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=TIMEOUT))
