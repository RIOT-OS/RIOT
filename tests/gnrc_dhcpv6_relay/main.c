/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "shell.h"

int main(void)
{
    char shell_buffer[SHELL_DEFAULT_BUFSIZE];

    /* start shell */
    shell_run(NULL, shell_buffer, sizeof(shell_buffer));
    return 0;
}

/** @} */
