/*
 * Copyright (C) 2013 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 Freie Universität Berlin
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       shows how to set up own and use the system shell commands.
 *              By typing help in the serial console, all the supported commands
 *              are listed.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 */

#include <stdio.h>
#include <string.h>

#include "shell.h"

/* define buffer to be used by the shell. Note: This is intentionally
 * smaller than 64 bytes, as the EDBG integrated UART bridge of the samr21-xpro
 * (and likely all other EDBG boards) drops chars when sending more than 64
 * bytes at a time. This results in the buffer overflow test failing. */
static char line_buf[16];

static int print_teststart(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("[TEST_START]\n");

    return 0;
}

static int print_testend(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("[TEST_END]\n");

    return 0;
}

static int print_shell_bufsize(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("%d\n", sizeof(line_buf));

    return 0;
}

static int print_echo(int argc, char **argv)
{
    for (int i = 0; i < argc; ++i) {
        printf("\"%s\"", argv[i]);
    }
    puts("");

    return 0;
}

static int print_empty(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "start_test", "starts a test", print_teststart },
    { "end_test", "ends a test", print_testend },
    { "bufsize", "Get the shell's buffer size", print_shell_bufsize },
    { "echo", "prints the input command", print_echo },
    { "empty", "print nothing on command", print_empty },
    { NULL, NULL, NULL }
};

int main(void)
{
    printf("test_shell.\n");

    shell_run(shell_commands, line_buf, sizeof(line_buf));

    return 0;
}
