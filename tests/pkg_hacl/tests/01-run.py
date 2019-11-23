#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


# increase the default timeout to 30s, on samr30-xpro this test takes 20s to
# complete.
TIMEOUT = 30


def testfunc(child):
    child.expect(r'OK \(\d+ tests\)', timeout=TIMEOUT)


if __name__ == "__main__":
    sys.exit(run(testfunc))
