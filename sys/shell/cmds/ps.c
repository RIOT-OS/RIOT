/*
 * SPDX-FileCopyrightText: 2013 INRIA.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for the PS module
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "ps.h"
#include "shell.h"

static int _ps_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    ps();

    return 0;
}

SHELL_COMMAND(ps, "Prints information about running threads.", _ps_handler);
