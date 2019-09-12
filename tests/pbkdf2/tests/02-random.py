#!/usr/bin/env python3
# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# Author: Juan Carrano <j.carrano@fu-berlin.de>
"""Random test vectors"""

from bisect import bisect as _bisect
from itertools import accumulate as _accumulate
import hashlib
import random as rand

import test_base


_pass_chars = [c for c in (chr(x) for x in range(128))
               if c.isprintable()]


class random2(rand.Random):
    # Murdock uses python 3.5 where random.choices is not available, this
    # is a verbatim copy from python 3.6
    def choices(self, population, weights=None, *, cum_weights=None, k=1):
        """Return a k sized list of population elements chosen with replacement.
        If the relative weights or cumulative weights are not specified,
        the selections are made with equal probability.
        """
        random = self.random
        if cum_weights is None:
            if weights is None:
                _int = int
                total = len(population)
                return [population[_int(random() * total)] for i in range(k)]
            cum_weights = list(_accumulate(weights))
        elif weights is not None:
            raise TypeError('Cannot specify both weights and cumulative weights')
        if len(cum_weights) != len(population):
            raise ValueError('The number of weights does not match the population')
        bisect = _bisect.bisect
        total = cum_weights[-1]
        hi = len(cum_weights) - 1
        return [population[bisect(cum_weights, random() * total, 0, hi)]
                for i in range(k)]


randgen = random2(42)


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
