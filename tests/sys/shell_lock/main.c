/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Test for the lock behaviour of the shell
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 */

#include <stdio.h>

#include "shell.h"

#include "test_utils/interactive_sync.h"

/* this command is used by the test automation to check whether commands are
 * executed or not (due to the shell being locked). */
static int _cmd_ping(int argc, char **argv)
{
    if (argc != 1) {
        printf("Usage: %s\n", argv[0]);
        return 1;
    }

    puts("PONG!");
    return 0;
}

SHELL_COMMAND(ping, "Echo \"PONG!\"", _cmd_ping);

int main(void)
{
    test_utils_interactive_sync();

    puts("test_shell_lock");

    /* define buffer to be used by the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    /* start shell */
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
