#!/usr/bin/env python3

#  Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>

import sys
from testrunner import run


def testfunc(child):
    child.expect("Test of behavior with NDEBUG: PASSED")
    child.expect("FAILED ASSERTION.")


if __name__ == "__main__":
    sys.exit(run(testfunc))
