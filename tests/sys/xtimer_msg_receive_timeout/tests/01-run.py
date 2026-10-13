#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect("[START]")
    for i in range(5):
        child.expect("Message: 42")
        child.expect("Timeout!")
    child.expect("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
