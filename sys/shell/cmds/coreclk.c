/*
 * SPDX-FileCopyrightText: 2023 Otto-von-Guericke Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
