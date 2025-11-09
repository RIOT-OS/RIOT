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

static int ot_console_cb(const char *abuf, uint16_t bufsize, void *context)
{
    (void) context;
    if (bufsize > 0)
    {
        printf("%.*s", bufsize, abuf);
    }
    return 0;
}

static int ot_cmd(int argc, char **argv)
{
    char command[SHELL_DEFAULT_BUFSIZE];
    for (int i = 1; i < argc; i++){
        if (i == 1)
        {
            strncpy(command, argv[i], SHELL_DEFAULT_BUFSIZE-1);
        }
        else
        {
            strncat(command, " ", SHELL_DEFAULT_BUFSIZE-strlen(command)-1);
            strncat(command, argv[i], SHELL_DEFAULT_BUFSIZE-strlen(command)-1);
        }
    }

    if (strlen(command) != 0)
    {
        otCliConsoleInputLine(command, SHELL_DEFAULT_BUFSIZE);
    }
    return 0;
}

SHELL_COMMAND(ot, "Use commands from OpenThread CLI", ot_cmd);

void ot_shell_init(otInstance *aInstance)
{
    otCliConsoleInit(aInstance, ot_console_cb, NULL);
}
