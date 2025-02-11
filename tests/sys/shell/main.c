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
#include <stdlib.h>

#include "thread.h"
#include "string_utils.h"
#include "ztimer.h"

#include "architecture.h"
#include "shell.h"

/* define buffer to be used by the shell. Note: This is intentionally
 * smaller than 64 bytes, as the EDBG integrated UART bridge of the samr21-xpro
 * (and likely all other EDBG boards) drops chars when sending more than 64
 * bytes at a time. This results in the buffer overflow test failing. */
static char line_buf[60];

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
    (void)argc;
    (void)argv;
    printf("%" PRIuSIZE "\n", sizeof(line_buf));

    return 0;
}

static int print_empty(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return 0;
}

/* The debug feature uses printf and needs more stack */
#if IS_USED(MODULE_STDIO_NIMBLE_DEBUG)
#  define STACKSIZE (THREAD_STACKSIZE_SMALL + THREAD_EXTRA_STACKSIZE_PRINTF)
#else
#  define STACKSIZE (THREAD_STACKSIZE_SMALL)
#endif
static char _stack[STACKSIZE];

static struct {
    uint16_t period_ms;
    uint16_t reps;
} _periodic_ctx;

static void *_func(void *arg)
{
    (void)arg;

    while (_periodic_ctx.reps--) {
        ztimer_sleep(ZTIMER_MSEC, _periodic_ctx.period_ms);
        puts("test");
    }

    return NULL;
}

/* test to make sure that waiting for stdin does not block other threads */
static int print_periodic(int argc, char **argv)
{
    if (argc > 1) {
        _periodic_ctx.reps = atoi(argv[1]);
    } else {
        _periodic_ctx.reps = 5;
    }

    _periodic_ctx.period_ms = 500;

    thread_create(_stack, sizeof(_stack), THREAD_PRIORITY_MAIN, 0,
                  _func, NULL, "periodic");

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "bufsize", "Get the shell's buffer size", print_shell_bufsize },
    { "start_test", "starts a test", print_teststart },
    { "end_test", "ends a test", print_testend },
    { "echo", "prints the input command", print_echo },
    { "empty", "print nothing on command", print_empty },
    { "periodic", "periodically print command", print_periodic },
    { NULL, NULL, NULL }
};

static int _xfa_test1(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("[XFA TEST 1 OK]\n");

    return 0;
}

static int _xfa_test2(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("[XFA TEST 2 OK]\n");

    return 0;
}

/* Add above commands to the shell commands XFA using helper macro.
 * Intentionally reversed order to test linker script based alphanumeric
 * ordering. */
SHELL_COMMAND(xfa_test2, "xfa test command 2", _xfa_test2);
SHELL_COMMAND(xfa_test1, "xfa test command 1", _xfa_test1);

int main(void)
{
    printf("test_shell.\n");

    /* define own shell commands */
    shell_run_once(shell_commands, line_buf, sizeof(line_buf));

    puts("shell exited");

    /* Restart the shell after the previous one exits, so that we can test
     * Ctrl-D exit */
    shell_run(shell_commands, line_buf, sizeof(line_buf));

    /* or use only system shell commands */
    /* shell_run(NULL, line_buf, sizeof(line_buf)); */

    return 0;
}
