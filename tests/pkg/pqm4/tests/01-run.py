#!/usr/bin/env python3

# Copyright (C) 2026 Mert Cakir <mert-cakir@outlook.com>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("pqm4 ML-DSA-44 self-test")
    child.expect_exact("[1/3] crypto_sign_keypair ...")
    child.expect_exact("  OK")
    child.expect_exact("[2/3] crypto_sign ...")
    child.expect_exact("  OK", timeout=30)
    child.expect_exact("[3/3] crypto_sign_open ...")
    child.expect_exact("  OK (message recovered and verified)")
    child.expect_exact("ALL TESTS PASSED")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=60))
