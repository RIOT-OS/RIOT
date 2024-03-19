/*
 * Copyright (C) 2019 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Shell-based test application for heap functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

static int malloc_cmd(int argc, char **argv)
{
    static void *ptr = 0;
    if (argc < 2) {
        printf("usage: %s <bytes>\n", argv[0]);
        return 1;
    }
    size_t size = atoi(argv[1]);
    ptr = malloc(size);
    printf("allocated %p\n", ptr);
    return 0;
}

static int free_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <ptr in hex> returned from malloc, e.g., 0x1234\n",
               argv[0]);
        return 1;
    }

    uintptr_t p = strtoul(argv[1], NULL, 16);
    void *ptr = (void *)p;
    printf("freeing %p\n", ptr);
    free(ptr);
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "malloc", "malloc <size>", malloc_cmd },
    { "free", "free <addr in hex> returned from malloc, e.g., 0x1234", free_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Shell-based test application for heap functions.\n"
         "Use the 'help' command to get more information on how to use it.");

    /* define buffer to be used by the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    /* define own shell commands */
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
