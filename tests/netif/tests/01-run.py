#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run, check_unittests


def testfunc(child):
    # embUnit tests
    check_unittests(child)


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1, traceback=True))
