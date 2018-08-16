#!/usr/bin/env python3

# Copyright (C) 2018 Acutam Automation, LLC
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("cb_mux test routine")
    child.expect_exact("Test list addition, retrieval, execution of 5 CBs")
    child.expect_exact("Callback 0 executed")
    child.expect_exact("Callback 1 executed")
    child.expect_exact("Callback 2 executed")
    child.expect_exact("Callback 3 executed")
    child.expect_exact("Callback 4 executed")
    child.expect_exact("Test list deletion of CB 0, 2, 4, execution of 1, 3")
    child.expect_exact("Callback 1 executed")
    child.expect_exact("Callback 3 executed")
    child.expect_exact("Test execution of CB with lowest ID (1)")
    child.expect_exact("Callback 1 executed")
    child.expect_exact("Test execution of CB with highest ID (3)")
    child.expect_exact("Callback 3 executed")
    child.expect_exact("Re-adding list entries (0, 2, 4) by finding next free ID")
    child.expect_exact("Added entry 0")
    child.expect_exact("Added entry 2")
    child.expect_exact("Added entry 4")
    child.expect_exact("Test iteration of a function over list")
    child.expect_exact("Entry 0 was updated correctly")
    child.expect_exact("Entry 1 was updated correctly")
    child.expect_exact("Entry 2 was updated correctly")
    child.expect_exact("Entry 3 was updated correctly")
    child.expect_exact("Entry 4 was updated correctly")
    child.expect_exact("Tests complete!")


if __name__ == "__main__":
    sys.exit(run(testfunc))
