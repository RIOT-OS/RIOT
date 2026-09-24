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


# Must match RIOT_VERSION_DUMMY_CODE in Makefile.include
DUMMY_CODE = (2042, 5, 23, 0)


def expected_code(version, extra):
    """
    Computes the parts of the version code that the build system should
    generate for the given version string and extra version.
    """

    parts = version.split("-", 1)[0].split(".")

    if len(parts) < 2 or not all(part.isdigit() for part in parts):
        return DUMMY_CODE

    major, minor, patch = (int(part) for part in (parts + ["0"])[:3])

    return (major, minor, patch, int(extra))


def testfunc(child):
    version = os.environ["RIOT_VERSION"]
    extra = os.environ["RIOT_EXTRAVERSION"]

    child.expect_exact("RIOT_VERSION: {}".format(version))
    child.expect_exact("RIOT_VERSION_CODE: {}.{}.{}.{}".format(
        *expected_code(version, extra)))

    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
