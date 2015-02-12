/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for mersenne twister
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "hwtimer.h"
#include "random.h"

void _mersenne_init(int argc, char **argv)
{
    int initval;

    if (argc == 1) {
        initval = hwtimer_now();
        printf("PRNG initialized to current time: %d\n", initval);
    }
    else {
        initval = atoi(argv[1]);
        printf("PRNG initialized given value: %d\n", initval);
    }

    genrand_init(initval);
}

void _mersenne_get(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    printf("%" PRIu32 "\n", genrand_uint32());
}
