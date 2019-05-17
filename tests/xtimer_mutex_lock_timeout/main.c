/*
 * Copyright (C) 2019 Freie Universitaet Berlin,
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
 * @brief       testing xtimer_mutex_lock_timeout function
 *
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "xtimer.h"

/**
 * Foward declarations
 */
static int cmd_test_xtimer_mutex_lock_timeout(int argc, char **argv);

/**
 * @brief   List of command for this application.
 */
static const shell_command_t shell_commands[] = {
    { "mutex_timeout_test", "tests xtimer mutex lock timeout", cmd_test_xtimer_mutex_lock_timeout, },
    { NULL, NULL, NULL }
};

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_mutex_lock_timeout(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: xtimer mutex lock timeout");
    mutex_t test_mutex = MUTEX_INIT;

    if (xtimer_mutex_lock_timeout(&test_mutex, XTIMER_BACKOFF * 3) == 0) {
        puts("OK");
    }
    else {
        puts("error: mutex timed out");
    }

    return 0;
}

/**
 * @brief   main function starting shell
 *
 * @return 0 on success
 */
int main(void)
{
    puts("Starting shell...");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
