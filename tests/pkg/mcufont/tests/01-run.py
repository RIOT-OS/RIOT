#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2023 Koen Zandberg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run

FONT_RENDER = """
                  █  █
 █     █          █  █                  █████   █    ███   ███████  █
 █     █          █  █                  █    █  █   █   █     █     █
 █     █   ████   █  █   ████           █    █  █  █     █    █     █
 █     █  ██  ██  █  █  ██  ██          █    █  █  █     █    █     █
 ███████  █    █  █  █  █    █          █████   █  █     █    █     █
 █     █  ██████  █  █  █    █          █   █   █  █     █    █     █
 █     █  █       █  █  █    █          █    █  █  █     █    █
 █     █  ██   █  █  █  ██  ██  █       █    █  █   █   █     █     █
 █     █   ████   █  █   ████   █       █     █ █    ███      █     █
                                █
"""


def testfunc(child):
    for line in FONT_RENDER.splitlines():
        child.expect_exact(line)
    print("\nSUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc))
