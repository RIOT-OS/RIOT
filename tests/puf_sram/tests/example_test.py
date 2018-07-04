#!/usr/bin/env python3

# Copyright (C) 2018 Kevin Weiss <kevin.weiss@haw-hamburg.de>
# Copyright (C) 2018 Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import puf_sram_if
import numpy


def min_erntropy(all_meas):
    p1 = numpy.zeros(len(all_meas[0]))
    # number of ones for each bit
    for i in range(0, len(all_meas[0])):
        tmp = list(map(lambda x: int(x[i]), all_meas))
        p1[i] = numpy.count_nonzero(tmp)

    # probability of ones
    p1 = numpy.divide(p1, float(len(all_meas)))
    # probability of zeros
    p0 = 1 - p1

    p0_1_max = numpy.maximum(p1, p0)
    log2_p0_1_max = numpy.log2(p0_1_max)
    H_min = numpy.sum(-log2_p0_1_max)
    H_min_rel = 100 * H_min/len(p0_1_max)

    return [H_min, H_min_rel]


def main_func():
    puf_sram = puf_sram_if.PufSram()
    seeds = puf_sram.get_seed_list(n=500, off_time=1, allow_print=True)
    seeds = [format(x, '0>32b') for x in seeds]
    H_min, H_min_rel = min_erntropy(seeds)

    print("Number of seeds: %i       " % len(seeds))
    print("Seed length    : %i Bit   " % len(seeds[0]))
    print("Abs. Entropy   : %02.02f Bit   " % H_min)
    print("Rel. Entropy   : %02.02f perc. " % H_min_rel)


if __name__ == "__main__":
    main_func()
