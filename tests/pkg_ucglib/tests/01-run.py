#!/usr/bin/env python3

# Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run

EXPECTED_STDOUT = (
    'ucg: UCG_MSG_DRAW_PIXEL (128)',
    'ucg: UCG_MSG_DRAW_PIXEL (256)',
    'ucg: UCG_MSG_DRAW_PIXEL (384)',
    'ucg: UCG_MSG_DRAW_PIXEL (512)',
    'ucg: UCG_MSG_DRAW_PIXEL (640)',
    'ucg: UCG_MSG_DRAW_PIXEL (768)',
    'ucg: UCG_MSG_DRAW_PIXEL (896)',
    'ucg: UCG_MSG_DRAW_PIXEL (1024)',
    'ucg: UCG_MSG_DRAW_PIXEL (1152)',
    'ucg: UCG_MSG_DRAW_PIXEL (1280)',
    'ucg: UCG_MSG_DRAW_PIXEL (1408)',
    'ucg: UCG_MSG_DRAW_PIXEL (1536)',
    'ucg: UCG_MSG_DRAW_PIXEL (1664)',
    'ucg: UCG_MSG_DRAW_PIXEL (1792)',
    'ucg: UCG_MSG_DRAW_PIXEL (1920)',
    'ucg: UCG_MSG_DRAW_PIXEL (2048)',
    'ucg: UCG_MSG_DRAW_PIXEL (2176)',
    'ucg: UCG_MSG_DRAW_PIXEL (2304)',
    'ucg: UCG_MSG_DRAW_PIXEL (2432)',
    'ucg: UCG_MSG_DRAW_PIXEL (2560)',
    'ucg: UCG_MSG_DRAW_PIXEL (2688)',
    'ucg: UCG_MSG_DRAW_PIXEL (2816)',
    'ucg: UCG_MSG_DRAW_PIXEL (2944)',
    'ucg: UCG_MSG_DRAW_PIXEL (3072)',
    'ucg: UCG_MSG_DRAW_PIXEL (3200)',
    'ucg: UCG_MSG_DRAW_PIXEL (3328)',
    'ucg: UCG_MSG_DRAW_PIXEL (3456)',
    'ucg: UCG_MSG_DRAW_PIXEL (3584)',
    'ucg: UCG_MSG_DRAW_PIXEL (3712)',
    'ucg: UCG_MSG_DRAW_PIXEL (3840)',
    'ucg: UCG_MSG_DRAW_PIXEL (3968)',
    'ucg: UCG_MSG_DRAW_PIXEL (4096)',
    'ucg: UCG_MSG_DRAW_PIXEL (4224)',
    'ucg: UCG_MSG_DRAW_PIXEL (4352)',
    'ucg: UCG_MSG_DRAW_PIXEL (4480)',
    'ucg: UCG_MSG_DRAW_PIXEL (4608)',
)


def testfunc(child):
    child.expect_exact('Initializing dummy output.')
    child.expect_exact('ucg: UCG_MSG_DEV_POWER_UP')
    child.expect_exact('ucg: UCG_MSG_GET_DIMENSION')
    child.expect_exact('Initializing display.')
    child.expect_exact('Drawing on screen.')

    for line in EXPECTED_STDOUT:
        child.expect_exact(line)


if __name__ == "__main__":
    sys.exit(run(testfunc))
