#!/usr/bin/env python3

# Copyright (C) 2018 Hauke Petersen <hauke.petersen@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    rt = child.expect([r"\[SUCCESS\]", r"\[FAILED\]"])
    if rt == 1:
        sys.exit(1)


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTTOOLS'], 'testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
