#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

"""
Test for passing `CFLAGS` with spaces to the application.

It also tests that even if a `CFLAGS` is set after including Makefile.include,
changing its value will trigger a rebuild.

There is also a way to test passing additional values with spaces to docker
documented in the `README.md`.
"""

import os
import sys
from testrunner import run


# Verify the macro matches the configuration value
CONFIGURATION_VALUE = os.environ['CONFIGURATION_VALUE']


def testfunc(child):
    child.expect_exact('The output of the configuration variables:')
    child.expect_exact('SUPER_STRING: I love sentences with spaces')
    child.expect_exact('DEFINED_AFTER_MAKEFILE_INCLUDE: %s' %
                       CONFIGURATION_VALUE)
    # This one is not tested here, see the output in 'riotbuild.h'
    child.expect(r'CFLAGS_STRING_FROM_DOCKER: .*')


if __name__ == "__main__":
    sys.exit(run(testfunc))
