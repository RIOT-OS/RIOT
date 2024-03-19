#!/usr/bin/env python3

# Copyright (C) 2019 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

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
