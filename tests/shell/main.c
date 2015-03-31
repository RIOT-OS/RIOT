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

#ifdef MODULE_READLINE
#include "readline.h"
#endif

#define SHELL_BUFSIZE   (UART0_BUFSIZE)

static int print_teststart(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    printf("[TEST_START]\n");

    return 0;
}

static int print_testend(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    printf("[TEST_END]\n");

    return 0;
}

static int print_echo(int argc, char **argv)
{
    for (int i = 0; i < argc; ++i) {
        printf("“%s” ", argv[i]);
    }
    puts("");

    return 0;
}

#ifdef USE_SHELL_COLORS
unsigned int FGBG[] = { 38, 48 };
static void print_colors(int argc, char **argv)
{
    (void) argc; (void) argv;
    int color, fgbg;
    for (fgbg = 0; fgbg < 2; fgbg++) {
        for (color = 0; color <= 256; color++) {
            printf("\033[%i;5;%im %i\t\033[0m", FGBG[fgbg], color, color);
            if (((color + 1) % 10) == 0) {
                putchar('\n');
            }
        }
        putchar('\n');
    }
    puts("\033[94m[Source: http://misc.flogisoft.com/bash/tip_colors_and_formatting]\033[0m");
}
#endif // USE_SHELL_COLORS

static int shell_readc(void)
{
    char c;
    int result = posix_read(uart0_handler_pid, &c, 1);
    if (result != 1) {
        return -1;
    }
    return (unsigned char) c;
}

static void shell_putchar(int c)
{
    putchar(c);
}

static const shell_command_t shell_commands[] = {
    { "start_test", "starts a test", print_teststart },
    { "end_test", "ends a test", print_testend },
    { "echo", "prints the input command", print_echo },
#ifdef USE_SHELL_COLORS
    { "colors", "Show fancy \033[31mC\033[32mo\033[34ml\033[34mo\033[35mr\033[36ms\033[37m!\033[0m", print_colors },
#endif
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
