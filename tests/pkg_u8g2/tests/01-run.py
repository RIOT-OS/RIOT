#!/usr/bin/env python3

# Copyright (C) 2017 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

EXPECTED_STDOUT = (
    '00|                                                   |',
    '02|                                                   |',
    '04|                                                   |',
    '06|                                                   |',
    '08|                                                   |',
    '10|      █████▐▋  ▐▋▐▋ ▟██▖                           |',
    '12|        █  ▐▋  ▐▋▐▋▐▛ ▝█                           |',
    '14|        █  ▐████▋▐▋▝█▙▄                            |',
    '16|        █  ▐▋  ▐▋▐▋  ▀▜▙                           |',
    '18|        █  ▐▋  ▐▋▐▋▐▙ ▗█                           |',
    '20|        █  ▐▋  ▐▋▐▋ ▜██▘                           |',
    '22|                                                   |',
    '24|                                                   |',
    '26|                                                   |',
    '28|                                                   |',
    '30|                                                   |',
    '00|                                                   |',
    '02|                                                   |',
    '04|                                                   |',
    '06|                                                   |',
    '08|                                                   |',
    '10|            ▐▋ ▟██▖                                |',
    '12|            ▐▋▐▛ ▝█                                |',
    '14|            ▐▋▝█▙▄                                 |',
    '16|            ▐▋  ▀▜▙                                |',
    '18|            ▐▋▐▙ ▗█                                |',
    '20|            ▐▋ ▜██▘                                |',
    '22|                                                   |',
    '24|                                                   |',
    '26|                                                   |',
    '28|                                                   |',
    '30|                                                   |',
    '00|           ▄▄▖                                     |',
    '02|         ▗████▋                                    |',
    '04|        ▗█▛▘ ▜█▖                                   |',
    '06|        ▐█    █▋                                   |',
    '08|        ██    █▋                                   |',
    '10|        ██   ▟█▘                                   |',
    '12|        ▜█ ▄▟█▛  ▄   ▄▄  ▄▄▄▄                      |',
    '14|    ▗▄█▖▐█▝██▀   █▋ ▟▛▜▙ ▀▜▛▀                      |',
    '16|   ▗██▀ ▐█ ▝     █▋ █▋ █  ▐▋                       |',
    '18|   ▟▛   ▐█  ▄    █▋ █▋ █  ▐▋                       |',
    '20|   █▋   ▐█ ▝█▋   █▋ █▋ █  ▐▋                       |',
    '22|   █▋   ▐█  ▜█   █▋ █▋ █  ▐▋                       |',
    '24|   █▙  ▗█▛  ▝█▙  █▋ █▋▗█  ▐▋                       |',
    '26|   ▐█▙▄██▘   ▜█  █▘ ▝██▘  ▐▋                       |',
    '28|    ▝▜█▀▘                                          |',
    '30|                                                   |',
)


def testfunc(child):
    child.expect_exact('Initializing to stdout.')
    child.expect_exact('Initializing display.')
    child.expect_exact('Drawing on screen.')

    for line in EXPECTED_STDOUT:
        child.expect_exact(line)


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
