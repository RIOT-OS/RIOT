#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
# SPDX-License-Identifier: BSD-2-Clause

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("This is DTLS loopback test client/server made with libcoap's response\r\n")


if __name__ == "__main__":
    sys.exit(run(testfunc))
