/*
 * Copyright (C) 2013  INRIA.
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
 * @brief       Shell commands for the PS module
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "ps.h"

#include "shell_commands.h"

int _ps_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    ps();

    return 0;
}

REGISTER_SHELL_COMMAND("ps", "Prints information about running threads.", _ps_handler);
