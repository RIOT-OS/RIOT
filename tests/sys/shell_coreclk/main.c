/*
 * Copyright (C) 2023 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
