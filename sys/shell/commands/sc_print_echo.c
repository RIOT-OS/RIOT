/*
 * Copyright (C) 2019 HAW Hamburg
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
 * @brief       Shell command to print echo.

 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 * @}
 */
#include <stdint.h>
#include <string.h>
#include "test_helper.h"

int _print_echo_handler(int argc, char **argv)
{
    print_echo(argc, argv);
    return 0;
}
