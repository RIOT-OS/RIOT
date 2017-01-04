/*
 * Copyright (C) 2016 Francisco Acosta <francisco.acosta@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

static void seed_rng(void)
{
    int fp = open("/dev/random", O_RDONLY);
    unsigned seed;
    unsigned pos = 0;

    if (fp == -1) {
        abort();
    }

    while (pos < sizeof(seed)) {
        int amt = read(fp, (char*)&seed + pos, sizeof(seed) - pos);

        if (amt <= 0) {
            abort();
        }

        pos += amt;
    }

    srand(seed);
    close(fp);
}

void randombytes(uint8_t *target, uint64_t n)
{
    seed_rng();

    while (n--) {
        *target++ = rand();
    }
}
