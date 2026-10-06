#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2021 Otto-von-Guericke-Universität Magdeburg
# SPDX-License-Identifier: LGPL-2.1-only

# @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"TEST ([A-Z]+)\r\n")
    assert child.match.group(1) == "PASSED"


if __name__ == "__main__":
    sys.exit(run(testfunc))
