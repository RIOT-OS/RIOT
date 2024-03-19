/*
 * Copyright (C) 2023 Otto-von-Guericke Universität Magdeburg
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
 * @brief       Shell command printing the CPU frequency
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "clk.h"
#include "shell.h"

static int _coreclk(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("core clock: %" PRIu32 " Hz\n", coreclk());
    return 0;
}

SHELL_COMMAND(coreclk, "Print the CPU frequency", _coreclk);
