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
 * @brief       freertos testing tool
 *
 * More detailed information about the file and the functionality implemented.
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "shell.h"

#include "freertos/FreeRTOS.h"
#include "semaphore_test.h"

/**
 * Foward declarations
 */
static int cmd_test_mutex(int argc, char **argv);
static int cmd_test_recursive_mutex(int argc, char **argv);


/**
 * @brief   List of command for this application.
 */
static const shell_command_t shell_commands[] = {
    { "mutex_semaphore", "tests freertos mutex semaphor", cmd_test_mutex, },
    { "recursive_mutex_semaphore", "tests freertos recursive mutex semaphor", cmd_test_recursive_mutex },
    { NULL, NULL, NULL }
};

/**
 * @brief   shell command to test freertos mutex semaphore
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_mutex(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: mutex semaphore");
    if (semaphore_test_mutex_take() == pdPASS) {
        puts("OK");
    }
    else {
        puts("mutex semaphore test failed");
    }

    return 0;
}

/**
 * @brief   shell command to test freertos recursive mutex semaphore
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_recursive_mutex(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: recursive mutex semaphore");
    if (semaphore_test_recursive_mutex_take() == pdPASS) {
        puts("OK");
    }
    else {
        puts("recursive mutex semaphore test failed");
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
