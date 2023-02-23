/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       CoRE Resource Directory lookup (cord_lc) example
 *
 * @author      Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

extern int cord_lc_cli_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "cord_lc", "Cord LC example", cord_lc_cli_cmd },
    { NULL, NULL, NULL },
};

int main(void)
{
    puts("CoRE RD lookup client example!\n");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
