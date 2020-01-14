#!/usr/bin/env python3

#
# Copyright (C) 2020 Sören Tempel <tempel@uni-bremen.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

import sys
import secrets

if len(sys.argv) >= 2:
    maxbits = int(sys.argv[1])
else:
    maxbits = 64

maxval = 2 ** maxbits
print(hex(secrets.randbelow(maxval)))
