#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect('TEST SUCCEEDED')


if __name__ == "__main__":
    sys.exit(run(testfunc))
