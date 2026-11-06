#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect('first thread started')
    child.expect('timer triggered')
    child.expect('first thread done')
    child.expect('TEST SUCCESSFUL')


if __name__ == "__main__":
    sys.exit(run(testfunc))
