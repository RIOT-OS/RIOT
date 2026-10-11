#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Xin He
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("nrf_sf_radio single-board test")
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
