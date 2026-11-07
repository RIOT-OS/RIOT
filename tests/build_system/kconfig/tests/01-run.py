#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 HAW Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Message 1 defined in app.config file")
    child.expect_exact("MSG_2 is active")
    child.expect_exact("External Message 1 defined in Kconfig file")
    child.expect_exact("External Message 2 defined in Kconfig file")
    child.expect_exact("External package message 1 defined in Kconfig file")
    child.expect_exact("External package message 2 defined in Kconfig file")


if __name__ == "__main__":
    sys.exit(run(testfunc))
