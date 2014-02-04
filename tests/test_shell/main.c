/*
 * Copyright (C) 2013 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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
#include <malloc.h>
#include <shell_commands.h>
#include <posix_io.h>
#include <shell.h>
#include <board_uart0.h>

#define SHELL_BUFSIZE   (UART0_BUFSIZE)

void print_teststart(char *unused)
{
    (void) unused;
    printf("[TEST_START]\n");

}

void print_testend(char *unused)
{
    (void) unused;
    printf("[TEST_END]\n");
}

int shell_readc(void)
{
    char c = 0;
    posix_read(uart0_handler_pid, &c, 1);
    return c;
}

void shell_putchar(int c)
{
    putchar(c);
}

const shell_command_t shell_commands[] = {
    { "start_test", "starts a test", print_teststart },
    { "end_test", "ends a test", print_testend },
    { NULL, NULL, NULL }
};

int main(void)
{

    printf("test_shell.\n");

    board_uart0_init();

    posix_open(uart0_handler_pid, 0);

    /* define own shell commands */
    shell_t shell;
    shell_init(&shell, shell_commands, SHELL_BUFSIZE, shell_readc,
               shell_putchar);
    shell_run(&shell);

    /* or use only system shell commands */
    /*
    shell_t sys_shell;
    shell_init(&sys_shell, NULL, SHELL_BUFSIZE, shell_readc, shell_putchar);
    shell_run(&sys_shell);
    */

    return 0;
}

