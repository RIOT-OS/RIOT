#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2018 Acutam Automation, LLC
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("cb_mux benchmark application")
    child.expect(r"Populating cb_mux list with \d+ items")
    child.expect_exact("Finding the last list entry")
    child.expect(r"List walk time: \d+ us")
    child.expect(r"Walk time less than threshold of \d+ us")
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
