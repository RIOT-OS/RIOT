#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Alexandre Abadie <alexandre.abadie@inria.fr>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


VALUE = 42
STRING = 'test'

STRING_FORMAT = "{}{}Logging value '{}' and string '{}'"
ERROR = '\x1b[1;31m'
WARNING = '\x1b[1;33m'
INFO = '\x1b[1m'
DEBUG = '\x1b[32m'
RESET = '\x1b[0m'

LEVELS = [ERROR, WARNING, INFO, DEBUG]


def testfunc(child):
    for level in LEVELS:
        child.expect_exact(STRING_FORMAT.format(RESET, level, VALUE, STRING))


if __name__ == "__main__":
    sys.exit(run(testfunc))
