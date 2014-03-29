/*
 * Copyright (C) 2013 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 Freie Universität Berlin
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
 *
 */

#include <stdio.h>
#include <string.h>

#include "shell_commands.h"
#include "posix_io.h"
#include "shell.h"
#include "board_uart0.h"

#define SHELL_BUFSIZE   (UART0_BUFSIZE)

static void print_teststart(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    puts("[TEST_START]");
}

static void print_testend(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    puts("[TEST_END]");
}

static void print_echo(int argc, char **argv)
{
    for (int i = 0; i < argc; ++i) {
        printf("“%s” ", argv[i]);
    }
    puts("");
}

static const shell_command_t shell_commands[] = {
    { "start_test", "starts a test", print_teststart },
    { "end_test", "ends a test", print_testend },
    { "echo", "prints the input command", print_echo },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("test_shell.");

    /* define own shell commands */
    shell_t shell;
    shell_init(&shell, shell_commands, SHELL_BUFSIZE, uart0_readc, uart0_putc);
    shell_run(&shell);

    /* or use only system shell commands */
    /*
    shell_t sys_shell;
    shell_init(&sys_shell, NULL, SHELL_BUFSIZE, shell_readc, shell_putchar);
    shell_run(&sys_shell);
    */

    return 0;
}
