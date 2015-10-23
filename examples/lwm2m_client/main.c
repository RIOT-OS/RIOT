/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     lwm2mclient
 * @{
 *
 * @file
 * @brief       LWM2M example implementation
 *
 * @author      Alexandru Razvan Caciulescu <alexandru.razvan.c@gmail.com>
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"

int main(void)
{

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
