#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run_check_unittests


# increase the default timeout to 30s, on samr30-xpro this test takes 20s to
# complete.
TIMEOUT = 30


if __name__ == "__main__":
    sys.exit(run_check_unittests(timeout=TIMEOUT))
