#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2018 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-FileCopyrightText: 2017 Sebastian Meiling <s@mlng.net>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"{ \"result\" : \d+(, \"ticks\" : \d+)? }")


if __name__ == "__main__":
    sys.exit(run(testfunc))
