/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell command implementation for OpenThread
 *
 * @author      Moritz Voigt <moritz.voigt@mailbox.tu-dresden.de>
 *
 * @}
 */
#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "openthread/cli.h"

static int ot_console_cb(void *context, const char *format, va_list args)
{
    (void)context;
    vprintf(format, args);
    return 0;
}

static int ot_cmd(int argc, char **argv)
{
    /* Join all arguments after "ot" into a single space-separated string */
    for (int i = 1; i < argc - 1; i++) {
        char *arg = argv[i];
        arg += strlen(arg);
        *arg = ' ';
    }

    if (strlen(argv[1]) != 0) {
        otCliInputLine(argv[1]);
    }
    else {
        otCliInputLine("help");
    }
    return 0;
}

SHELL_COMMAND(ot, "Use commands from OpenThread CLI", ot_cmd);

void ot_shell_init(otInstance *aInstance)
{
    otCliInit(aInstance, ot_console_cb, NULL);
}
