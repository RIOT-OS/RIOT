#!/usr/bin/env python3
# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# Author: Juan Carrano <j.carrano@fu-berlin.de>
"""Random test vectors"""

import hashlib
import random

import test_base

randgen = random.Random(42)

_pass_chars = [c for c in (chr(x) for x in range(128))
               if c.isprintable()]


def randompass(length):
    return "".join(randgen.choices(_pass_chars, k=length))


def randomsalt(bytes_):
    return (randgen.getrandbits(bytes_*8).to_bytes(bytes_, 'big')
            if bytes_ else b'')


def randomvector(pass_len, salt_len, iters):
    pass_ = randompass(pass_len)
    salt = randomsalt(salt_len)
    key = hashlib.pbkdf2_hmac('sha256', pass_.encode('ascii'), salt, iters)

    return pass_, salt, iters, key


VECTORS = [
    randomvector(0, 16, 10),
    randomvector(8, 0, 10),
    randomvector(9, 64, 1),
    randomvector(65, 38, 20),
    randomvector(32, 15, 12),
    randomvector(48, 32, 15),
    ]


if __name__ == "__main__":
    test_base.main(VECTORS)
