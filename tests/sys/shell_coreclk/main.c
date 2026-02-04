/*
 * SPDX-FileCopyrightText: 2023 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Test for the coreclk shell command
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#include <stdio.h>

#include "shell.h"

int main(void)
{
    /* define buffer to be used by the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    /* start shell */
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
