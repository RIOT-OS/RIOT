#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8

# Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
#               2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"TEST_HZ=\d+\r\n")
    child.expect_exact("[START]\r\n")
    for _ in range(10):
        child.expect_exact(".\r\n")

    child.expect(r"drift: min=-?\d+ max=-?\d+ final=-?\d+\r\n")
    child.expect(r"jitter: min=-?\d+ max=-?\d+ abs avg=\d+\r\n")

    child.expect_exact("[DONE]\r\n")


if __name__ == "__main__":
    sys.exit(run(testfunc))
