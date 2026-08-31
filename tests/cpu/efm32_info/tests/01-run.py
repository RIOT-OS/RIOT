#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect("Series: [012]")
    child.expect("Architecture: cortex-m(0plus|33|3|4f)")

    child.expect("Crypto: [yn]")
    child.expect("TRNG: [yn]")
    child.expect("Radio: [yn]")

    child.expect_exact("Testing the flash layout.")
    child.expect("Flash: 0x[0-9a-f]{8} - 0x[0-9a-f]{8}")

    child.expect_exact("Testing the SRAM layout.")
    child.expect("SRAM: 0x[0-9a-f]{8} - 0x[0-9a-f]{8}")

    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
