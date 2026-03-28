#!/usr/bin/env python3

#  Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>

import sys
from testrunner import run


# use a larger timeout as context switches make take some time on slower MCUs
TIMEOUT = 60


def testfunc(child):
    child.expect("Testing 500 context switches triggered from ISR")
    child.expect("TEST PASSED", timeout=TIMEOUT)


if __name__ == "__main__":
    sys.exit(run(testfunc))
