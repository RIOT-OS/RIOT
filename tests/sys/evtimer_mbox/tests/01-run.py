#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Simon Brummer <simon.brummer@posteo.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect('Test successful')


if __name__ == '__main__':
    sys.exit(run(testfunc, timeout=1, echo=True))
