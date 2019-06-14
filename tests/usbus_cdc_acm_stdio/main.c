/*
 * Copyright (C) 2019 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Basic test for USB CDC ACM functionality. When plugged into a
 *              USB port, the peripheral should show up as a serial modem USB
 *              peripheral (/dev/ttyACMx on Linux) and should present the RIOT
 *              shell over this serial device.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 */

#include <stdio.h>

#include "shell.h"
#include "shell_commands.h"

int main(void)
{
    (void) puts("RIOT USB CDC ACM shell test");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
