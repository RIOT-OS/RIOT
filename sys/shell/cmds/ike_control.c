/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for the IKE module
 *
 * @author      Juliusz Neuman <neuman.juliusz@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "ike/ike.h"
#include "shell.h"

int _parse_ike_cmd(int argc, char **argv);
static void _print_ike_usage(char *cmdname);

int _parse_ike_cmd(int argc, char **argv)
{
    char *cmdname = argv[0];
    int res = 1;

    /* parse command line arguments */
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] != '-') {
            break;
        }
        switch (arg[1]) {
        case 'h':
            _print_ike_usage(cmdname);
            res = 0;
            break;
        case 'i':
            if (i + 1 >= argc) {
                puts("No peer address given");
                break;
            }
            res = ike_init(argv[i + 1]);
            break;
        case 't':
            if (i + 1 != argc) {
                puts("Too many arguments");
                break;
            }
            res = ike_terminate();
            break;
        case 'l':
            if (i + 2 >= argc) {
                puts("Not enough arguments");
                break;
            }
            for (int j = 0; j < atoi(argv[i + 2]); ++j)
            {
                printf("Round %d\n", j);
                res = ike_init(argv[i + 1]);
                res |= ike_terminate();
                if (res)
                    break;
            }
            break;
        default:
            break;
        }
    }
    if (res != 0) {
        _print_ike_usage(cmdname);
    }
    return res;

}

SHELL_COMMAND(ike_control, "Command for managing IPSec", _parse_ike_cmd);

static void _print_ike_usage(char *cmdname)
{
    printf("%s [-i addrs]\n",
           cmdname);
    puts("     -i - initialize IKE SA to addrs");
    printf("%s [-t]\n",
           cmdname);
    puts("     -t - terminate established IKE SA");
    printf("%s [-l addrs num]\n",
           cmdname);
    puts("     -l - establish and terminate IKE SA in loop num times to addrs");
}
