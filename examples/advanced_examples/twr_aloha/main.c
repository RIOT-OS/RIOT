/*
 * Copyright (C) 2020 Inria
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
 * @brief       Two way ranging example
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include "shell.h"

#include "control.h"

extern int _twr_handler(int argc, char **argv);
extern int _twr_ifconfig(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "twr", "Two-way-ranging (TWR) cli", _twr_handler },
    { "ifconfig", "Network interface information", _twr_ifconfig},
    { NULL, NULL, NULL }
};

int main(void)
{
    /* this should start ranging... */
    uwb_core_rng_init();
    /* define buffer to be used by the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 1;
}
