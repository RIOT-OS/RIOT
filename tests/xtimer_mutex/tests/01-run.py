#!/usr/bin/env python3

# Copyright (C) 2017 TriaGnoSys GmbH
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner

def testfunc(child):
    child.expect(u"main: done")
    child.expect(u"thread1: done")
    child.expect(u"thread2: done")

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
