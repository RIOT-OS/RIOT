/*
 * SPDX-FileCopyrightText: 2015 Daniel Krebs
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for AT30TSE75x temperature sensor
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"

int main(void)
{
    puts("AT30TSE75x device driver test");

    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
