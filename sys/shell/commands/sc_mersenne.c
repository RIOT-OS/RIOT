/*
 * Copyright (C) 2013 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#ifdef MODULE_XTIMER
#include "xtimer.h"
#endif

#include "random.h"

int _mersenne_init(int argc, char **argv)
{
    int initval;

    if (argc == 1) {
#ifdef MODULE_XTIMER
        initval = xtimer_now();
        printf("PRNG initialized to current time: %d\n", initval);
#else
        (void)initval;
        printf("xtimer module not compiled in, can't initialize by time.\n");
#endif
    }
    else {
        initval = atoi(argv[1]);
        printf("PRNG initialized given value: %d\n", initval);
    }

    genrand_init(initval);

    return 0;
}

int _mersenne_get(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    printf("%" PRIu32 "\n", genrand_uint32());

    return 0;
}
