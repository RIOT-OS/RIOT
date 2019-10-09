/*
 * Copyright (C) 2019 Inria
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
 * @brief       Shell commands interactive sync util
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <stdio.h>
#include "test_utils/interactive_sync.h"

int _test_start(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    puts("START");

    return 0;
}


int _test_ready(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    puts("READY");
    return 0;
}
