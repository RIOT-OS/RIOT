/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for xtimer API
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "shell.h"
#include "xtimer.h"


int cmd_xtimer_now(int argc, char **argv)
{
    (void)argv;
    (void)argc;

    uint32_t now = xtimer_now().ticks32;
    printf("Success: xtimer_now(): [%"PRIu32"]\n", now);
    return 0;
}

int cmd_get_metadata(int argc, char **argv)
{
    (void)argv;
    (void)argc;

    printf("Success: [%s, %s]\n", RIOT_BOARD, RIOT_APPLICATION);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "xtimer_now", "Get number of ticks (32Bit) from xtimer", cmd_xtimer_now },
    { "get_metadata", "Get the metadata of the test firmware", cmd_get_metadata },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Start: Test for the xtimer API");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
