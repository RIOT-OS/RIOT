#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Alexandre Abadie <alexandre.abadie@inria.fr>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


VALUE = 42
STRING = 'test'

STRING_FORMAT = '{}{}Logging value \'{}\' and string \'{}\''
ERROR = '\033[1;31m'
WARNING = '\033[1;33m'
INFO = '\033[1m'
DEBUG = '\033[0;32m'
RESET = '\033[0m'

LEVELS = [ERROR, WARNING, INFO, DEBUG]


def testfunc(child):
    for level in LEVELS:
        child.expect_exact(STRING_FORMAT.format(RESET, level, VALUE, STRING))


if __name__ == "__main__":
    sys.exit(run(testfunc))
