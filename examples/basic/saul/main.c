/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example for demonstrating SAUL and the SAUL registry
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"

int main(void)
{
    puts("Welcome to RIOT!\n");
    puts("Type `help` for help, type `saul` to see all SAUL devices\n");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
