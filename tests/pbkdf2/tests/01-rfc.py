#!/usr/bin/env python3
# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# Author: Juan Carrano <j.carrano@fu-berlin.de>
"""Vector from RFC 7914 section 11"""

import os

import hashlib
import test_base

KEY_SIZE = hashlib.sha256().digest_size

v_easy = """55 ac 04 6e 56 e3 08 9f ec 16 91 c2 25 44 b6 05
   f9 41 85 21 6d de 04 65 e6 8b 9d 57 c2 0d ac bc
   49 ca 9c cc f1 79 b6 45 99 16 64 b3 9d 77 ef 31
   7c 71 b8 45 b1 e3 0b d5 09 11 20 41 d3 a1 97 83"""

v_hard = """
   4d dc d8 f6 0b 98 be 21 83 0c ee 5e f2 27 01 f9
   64 1a 44 18 d0 4c 04 14 ae ff 08 87 6b 34 ab 56
   a1 d4 25 a1 22 58 33 54 9a db 84 1b 51 c9 b3 17
   6a 27 2b de bb a1 d0 78 47 8f 62 b3 97 f3 3c 8d"""


def process_octets(s):
    return bytes(int(x, 16) for x in s.split())[:KEY_SIZE]


VECTORS = [
    ('passwd', b"salt", 1, process_octets(v_easy))
    ]

if os.environ.get('BOARD') == 'native':
    VECTORS.append(("Password", b"NaCl", 80000, process_octets(v_hard)))

if __name__ == "__main__":
    test_base.main(VECTORS)
