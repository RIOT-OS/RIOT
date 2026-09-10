#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact(
        "Type characters to echo them, or wait for the periodic timer.")

    # sending input should wake the thread through the notify callback and
    # echo the bytes back via stdio_read
    child.sendline("hello 123")
    child.expect("hello 123")

    # without any input, the periodic timer should still wake the thread
    child.expect_exact("*** Woken up! ***", timeout=10)

    # sending more input to assert that thread is still capable of waking up
    child.sendline("world 456")
    child.expect("world 456")


if __name__ == "__main__":
    sys.exit(run(testfunc))
