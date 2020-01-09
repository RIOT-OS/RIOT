#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    board = os.environ['BOARD']
    # Increase timeout on "real" hardware
    # 170 seconds on `arduino-mega2560`
    timeout = 200 if board != 'native' else -1
    child.expect(r"OK \(2 tests\)", timeout=timeout)


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'],
                                 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
