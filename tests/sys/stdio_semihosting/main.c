/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       STDIO Semihosting test application using simple shell
 *              interaction
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"

int main(void)
{
    puts("STDIO semihosting test application");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
