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
 * @brief       Shell command for getting application metadata.

 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 * @}
 */

#include "app_metadata.h"

int _app_metadata_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    app_metadata_print_json();
    return 0;
}
