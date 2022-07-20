#!/usr/bin/env python3

# Copyright (C) 2022 Benjamin Valentin <benjamin.valentin@ml-pa.com>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect('first thread')
    child.expect('second thread')
    child.expect('> wake sleeping threads')
    child.expect('first thread')
    child.expect('second thread')
    child.expect('> put threads to sleep again')


if __name__ == "__main__":
    sys.exit(run(testfunc))
