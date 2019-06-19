/*
 * Copyright (C) 2018 Unwired Devices LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for Cortex-M address checks
 *
 * @author      Oleg Artamonov <oleg@unwds.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cpu.h"
#include "shell.h"

static int cmd_check(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <hex address>\n", argv[0]);
        printf("\t example: %s 0x08080000\n", argv[0]);
        return 1;
    }

    char *address;
    uint32_t address_value = strtoul(argv[1], NULL, 0);

    address = (char *)address_value;

    if (cpu_check_address(address))
        printf("Address 0x%08" PRIx32 " is valid. Feel free to access it\n", address_value);
    else
        printf("Address 0x%08" PRIx32 " is NOT valid. Accessing it will result in BusFault\n", address_value);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "check", "Check address", cmd_check},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Address check test\n");
    puts("Please refer to the README.md for more details\n");
    puts("usage: check <hex address>");
    puts("\texample: check 0x08080000");

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
