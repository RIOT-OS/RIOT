/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
