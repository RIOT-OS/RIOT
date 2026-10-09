#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Freie Universität Berlin,
# SPDX-License-Identifier: LGPL-2.1-only

# @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>

import sys
from testrunner import run


def testfunc(child):
    child.expect("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc))
