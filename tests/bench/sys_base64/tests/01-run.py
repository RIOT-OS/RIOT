#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Verifying that base64 encoding works for benchmark input: OK\r\n")
    child.expect_exact("Verifying that base64 decoding works for benchmark input: OK\r\n")
    child.expect(r"Encoding 1\.000 x 96 bytes \(128 bytes in base64\): [0-9]+ µs\r\n")
    child.expect(r"Decoding 1\.000 x 96 bytes \(128 bytes in base64\): [0-9]+ µs\r\n")


if __name__ == "__main__":
    sys.exit(run(testfunc))
