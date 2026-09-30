#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
# SPDX-License-Identifier: LGPL-2.1-only

"""
Test for the parsing of `RIOT_VERSION` and `RIOT_EXTRAVERSION` into the
`RIOT_VERSION_CODE` macro.

The expected version code is computed from the same variables that the build
system used, and compared to the version code that the application prints.
"""

import os
import sys
from testrunner import run


def expected_code(version, extra, dummy):
    """
    Computes the parts of the version code that the build system should
    generate for the given version string, extra version and comma separated
    dummy version.
    """

    parts = version.split("-", 1)[0].split(".")

    if len(parts) < 2 or not all(part.isdigit() for part in parts):
        parts = dummy.split("-", 1)[0].split(".")

        if (len(parts) < 2 or not all(part.isdigit() for part in parts)):
            raise ValueError("RIOT_DUMMY_VERSION is not a valid version.")

    major, minor, patch = (int(part) for part in (parts + ["0"])[:3])

    return (major, minor, patch, int(extra))


def testfunc(child):
    version = os.environ["RIOT_VERSION"]
    extra = os.environ["RIOT_EXTRAVERSION"]
    dummy = os.environ["RIOT_DUMMY_VERSION"]

    child.expect_exact("RIOT_VERSION: {}".format(version))
    child.expect_exact("RIOT_VERSION_CODE: {}.{}.{}.{}".format(
        *expected_code(version, extra, dummy)))

    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
