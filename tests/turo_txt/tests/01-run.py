#!/usr/bin/env python3

# Copyright (C) 2017 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


PASSING_CMDS = [
    'turo_simple_s32 0',
    'turo_simple_s32 -1',
    'turo_simple_s32 0x7FFFFFFF',
    'turo_simple_s32 -2147483648',
    'turo_simple_array_u8 1',
    'turo_simple_array_u8 2 255',
    'turo_simple_array_s32 0 -1 0x7FFFFFFF -2147483648',
    'turo_simple_dict_string foo bar',
    'turo_simple_dict_s32 baz -2147483648',
    'turo_simple_exit_status 0',
    'test_multi_element_dict a b c d',
    'test_netif'
]


FAILING_CMDS = [
    'turo_simple_s32 a',
    'turo_simple_s32 0x100000000',
    'turo_simple_s32 -2147483649',
    'turo_simple_array_u8 -1',
    'turo_simple_array_u8 2 256',
    'turo_simple_array_s32 a b',
    'turo_simple_array_s32 -2147483649',
    'turo_simple_dict_s32 baz s',
    'turo_simple_dict_s32 baz -2147483649',
    'turo_simple_exit_status 1'
]


def testfunc(child):

    # Simple checking if the exit code works across everything
    for cmd in PASSING_CMDS:
        child.sendline(cmd)
        child.expect('Success')
    for cmd in FAILING_CMDS:
        child.sendline(cmd)
        child.expect('Error')


if __name__ == "__main__":
    sys.exit(run(testfunc))
