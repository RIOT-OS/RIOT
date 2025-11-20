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
#include <stdlib.h>

#include "shell.h"

static int cmd_text(int argc, char **argv)
{
    char *usage = "text [length]\n";
    if (argc != 2) {
        puts(usage);
        return -1;
    }
    int n = atoi(argv[1]);
    if (n <= 0) {
        puts(usage);
        return -1;
    }
    for (int i=0; i < n; i++) {
        if (i && (i % 80 == 0)) {
            puts("");
        }
        putc('0' + (i % 10), stdout);
    }
    puts("");
    return 0;
}

SHELL_COMMAND(text, "Generates long text for testing stdio buffer", cmd_text);

int main(void)
{
    (void) puts("RIOT USB CDC ACM shell test");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
