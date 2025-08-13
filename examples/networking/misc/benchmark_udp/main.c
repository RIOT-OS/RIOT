/*
 * SPDX-FileCopyrightText: 2021 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for exercising the RIOT network stack
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"

int main(void)
{
    puts("RIOT UDP stress-test application");

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, sizeof(line_buf));

    /* should be never reached */
    return 0;
}
