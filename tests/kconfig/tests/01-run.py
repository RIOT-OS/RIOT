#!/usr/bin/env python3

# Copyright (C) 2019 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Message 1 defined in app.config file")
    child.expect_exact("MSG_2 is active")
    child.expect_exact("External Message 1 defined in Kconfig file")
    child.expect_exact("External Message 2 defined in Kconfig file")


if __name__ == "__main__":
    sys.exit(run(testfunc))
