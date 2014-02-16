/**
 * Shell commands for mersenne twister
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_mersenne.c
 * @brief   initializes the PRNG
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "hwtimer.h"
#include "random.h"

void _mersenne_init(char *str)
{
    int initval;
    char *toc_str = strtok(str, " ");

    toc_str = strtok(NULL, " ");
    if (!toc_str) {
        initval = hwtimer_now();
        printf("PRNG inizialized to current time: %d\n", initval);
    }
    else {
        initval = atoi(toc_str);
        printf("PRNG inizialized given value: %d\n", initval);
    }

    genrand_init(initval);
}

void _mersenne_get(char *str)
{
    (void) str;

    printf("%" PRIu32 "\n", genrand_uint32());
}
