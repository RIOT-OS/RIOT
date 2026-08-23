/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "shell.h"

static int _app_metadata_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    app_metadata_print_json();
    return 0;
}

SHELL_COMMAND(app_metadata, "Returns application metadata", _app_metadata_handler);
