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
import argparse


def secrets_random():
    maxval = 2 ** 32
    return secrets.randbelow(maxval) / maxval


parser = argparse.ArgumentParser()
parser.add_argument('--seed', metavar='SEED', type=int,
                    default=-1, help='Seed used for random number generator')
args = parser.parse_args()

randfn = secrets_random
if args.seed != -1:
    random.seed(a=args.seed)
    randfn = random.random

input = sys.stdin.readlines()
random.shuffle(input, randfn)

for line in input:
    print(line, end='')
