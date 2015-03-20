/*
 * Copyright (C) 2014  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
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
 * @brief       Shell commands for system calls
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include "kernel.h"

int _reboot_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    (void) reboot(RB_AUTOBOOT);

    return 0;
}
