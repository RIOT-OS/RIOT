/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file        main.c
 * @brief       Example application to demonstrate the usage of the shell over standard IO
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "shell_commands.h"

#define SHELL_BUFFER_SIZE       128

static int shell_read(void);
static void shell_write(int);
static void cmd_hello(int argc, char **argv);
static void cmd_echo(int argc, char **argv);

/**
 * define some additional shell commands
 */
static const shell_command_t shell_commands[] = {
    { "hello", "say hello", cmd_hello },
    { "echo", "echo the value given", cmd_echo },
    { NULL, NULL, NULL }
};

/**
 * @brief say hello
 */
void cmd_hello(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("Hello to you, too!");         // TODO -> this should be printed through shell->put_char()
}

/**
 * @brief echo the input back to the output
 */
void cmd_echo(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);          // TODO -> this should be printed through shell->put_char()
    }
    printf("\n");                       // TODO -> this should be printed through shell->put_char()
}

/**
 * @brief proxy for reading a char from std-in and passing it to the shell
 */
int shell_read(void)
{
    return (int)getchar();
}

/**
 * @brief proxy for taking a character from the shell and writing it to std-out
 */
void shell_write(int c)
{
    putchar((char)c);
}

/**
 * @brief run Forrest, run!
 */
int main(void)
{
    shell_t shell;

    puts("Welcome to RIOT!");

    puts("Initializing shell...");
    shell_init(&shell, shell_commands, SHELL_BUFFER_SIZE, shell_read, shell_write);

    puts("Starting shell...");
    shell_run(&shell);
    return 0;
}
