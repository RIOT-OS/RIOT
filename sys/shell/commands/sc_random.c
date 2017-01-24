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
 * @brief       Shell commands for random generators
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

int _random_init(int argc, char **argv)
{
    int initval;

    if (argc == 1) {
#ifdef MODULE_XTIMER
        initval = _xtimer_now();
        printf("PRNG initialized to current time: %d\n", initval);
#else
        (void)initval;
        puts("xtimer module not compiled in, can't initialize by time.\n"
             "Please provide a seed.\n");
        return 1;
#endif
    }
    else {
        initval = atoi(argv[1]);
        printf("PRNG initialized given value: %d\n", initval);
    }

    random_init(initval);

    return 0;
}

int _random_get(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    printf("%" PRIu32 "\n", random_uint32());

    return 0;
}
