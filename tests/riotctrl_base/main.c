/*
 * main.c
 * Copyright (C) 2021 Martine Lenders <mail@martine-lenders.eu>
 *
 * Distributed under terms of the MIT license.
 */

#include "shell.h"

static int _empty(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    return 0;
}

static const shell_command_t _shell_commands[] = {
    { "empty", "A command with empty output", _empty },
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(_shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
