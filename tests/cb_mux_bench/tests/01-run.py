#!/usr/bin/env python3

# Copyright (C) 2018 Acutam Automation, LLC
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("cb_mux benchmark application")
    child.expect(u"Populating cb_mux list with \d+ items")
    child.expect_exact("Finding the last list entry")
    child.expect(u"List walk time: \d+ us")
    child.expect(u"Walk time less than threshold of \d+ us")
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
