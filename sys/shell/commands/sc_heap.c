/*
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
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
 * @brief       Print the state of the heap
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @}
 */

#include "shell_commands.h"

extern void heap_stats(void);

int _heap_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    heap_stats();

    return 0;
}

REGISTER_SHELL_COMMAND("heap", "Shows the heap state for the LPC2387 on the command shell.", _heap_handler);
