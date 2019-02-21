#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


# increase the default timeout to 20s, on samr30-xpro this test takes 14s to
# complete.
TIMEOUT = 20


def testfunc(child):
    child.expect(r"OK \(2 tests\)", timeout=TIMEOUT)


if __name__ == "__main__":
    sys.exit(run(testfunc))
