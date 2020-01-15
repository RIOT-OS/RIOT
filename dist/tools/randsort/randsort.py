#!/usr/bin/env python3

#
# Copyright (C) 2020 Sören Tempel <tempel@uni-bremen.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

import sys
import random
import secrets

def secrets_random():
    maxval = 2 ** 32
    return secrets.randbelow(maxval) / maxval

input = sys.stdin.readlines()
random.shuffle(input, secrets_random)

for line in input:
    print(line, end='')
