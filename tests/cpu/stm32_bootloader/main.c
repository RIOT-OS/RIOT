/*
 * SPDX-FileCopyrightText: 2020 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application to activate the STM32 bootloader mode.
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "shell.h"

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    puts("STM32 bootloader test application.");

    /* bootloader command is provided by default */
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
