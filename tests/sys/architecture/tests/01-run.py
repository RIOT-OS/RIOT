#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
# SPDX-License-Identifier: LGPL-2.1-only

# @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>

import sys
from testrunner import run


def testfunc(child):
    # Try to wait for the shell
    child.expect_exact("TEST SUCCEEDED")


if __name__ == "__main__":
    sys.exit(run(testfunc))
