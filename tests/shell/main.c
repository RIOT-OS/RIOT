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
#include "shell.h"

#if MODULE_STDIO_RTT
#include "xtimer.h"
#endif

#if MODULE_SHELL_HOOKS
void shell_post_readline_hook(void)
{
    puts("shell_post_readline_hook");
}

void shell_pre_command_hook(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("shell_pre_command_hook");
}

void shell_post_command_hook(int ret, int argc, char **argv)
{
    (void)ret;
    (void)argc;
    (void)argv;
    puts("shell_post_command_hook");
}
#endif

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
        printf("\"%s\"", argv[i]);
    }
    puts("");

    return 0;
}

static int print_shell_bufsize(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    printf("%d\n", SHELL_DEFAULT_BUFSIZE);

    return 0;
}

static int print_empty(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "bufsize", "Get the shell's buffer size", print_shell_bufsize },
    { "start_test", "starts a test", print_teststart },
    { "end_test", "ends a test", print_testend },
    { "echo", "prints the input command", print_echo },
    { "empty", "print nothing on command", print_empty },
    { NULL, NULL, NULL }
};

static int _xfa_test1(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    printf("[XFA TEST 1 OK]\n");

    return 0;
}

static int _xfa_test2(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    printf("[XFA TEST 2 OK]\n");

    return 0;
}

/* Add above commands to the shell commands XFA using helper macro.
 * Intentionally reversed order to test linker script based alphanumeric
 * ordering. */
SHELL_COMMAND(xfa_test2, "xfa test command 2",_xfa_test2);
SHELL_COMMAND(xfa_test1, "xfa test command 1",_xfa_test1);

int main(void)
{
    printf("test_shell.\n");

    /* define buffer to be used by the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    /* define own shell commands */
    shell_run_once(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    puts("shell exited");

    /* Restart the shell after the previous one exits, so that we can test
     * Ctrl-D exit */
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* or use only system shell commands */
    /*
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
    */

    return 0;
}
