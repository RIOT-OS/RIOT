#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-FileCopyrightText: 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run_check_unittests


# increase the default timeout to 20s, on samr30-xpro this test takes 14s to
# complete.
TIMEOUT = 20


if __name__ == "__main__":
    sys.exit(run_check_unittests(timeout=TIMEOUT, nb_tests=2))
