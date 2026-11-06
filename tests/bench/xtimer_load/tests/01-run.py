#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8

# SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

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
