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

#ifdef MODULE_RANDOM_SEED
#include "random_seed.h"
#endif

#include "random.h"

int _random_init(int argc, char **argv)
{
    uint32_t initval;

    if (argc == 1) {
#ifdef MODULE_RANDOM_SEED
        initval = random_prng_seed();
        printf("PRNG initialized to: %" PRIu32 "\n", initval);
#else
        (void)initval;
        puts("random_seed module not compiled in, can't automatically initialize.\n"
             "Please provide a seed.\n");
        return 1;
#endif
    }
    else {
        initval = atoi(argv[1]);
        printf("PRNG initialized given value: %" PRIu32 "\n", initval);
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
