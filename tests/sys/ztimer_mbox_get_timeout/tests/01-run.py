#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2023 Otto-von-Guericke-Universität Magdeburg
# SPDX-License-Identifier: LGPL-2.1-only

# @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>

import sys
from testrunner import run


def testfunc(child):
    child.expect("Testing ztimer_mbox_get_timeout()")
    child.expect("ALL TESTS SUCCEEDED")


if __name__ == "__main__":
    sys.exit(run(testfunc))
