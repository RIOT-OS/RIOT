/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stddef.h>
#include "shell.h"

int main(void)
{
    char shell_buffer[SHELL_DEFAULT_BUFSIZE];

    /* start shell */
    shell_run(NULL, shell_buffer, sizeof(shell_buffer));
    return 0;
}

/** @} */
