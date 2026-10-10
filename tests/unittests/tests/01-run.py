#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run_check_unittests


TIMEOUT = 120


if __name__ == "__main__":
    sys.exit(run_check_unittests(timeout=TIMEOUT))
