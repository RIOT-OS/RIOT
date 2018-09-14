/*
 * Copyright (C) 2015 Daniel Krebs
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
 * @brief       Test application for AT30TSE75x temperature sensor
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "shell_commands.h"

int main(void)
{
    puts("AT30TSE75x device driver test");

    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
