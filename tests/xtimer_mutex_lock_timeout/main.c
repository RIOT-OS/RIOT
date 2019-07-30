/*
 * Copyright (C) 2019 Freie Universität Berlin,
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
#include "shell.h"
#include "xtimer.h"

/* timeout at one millisecond (1000 us) to make sure it does not spin. */
#define LONG_MUTEX_TIMEOUT 1000

/**
 * Foward declarations
 */
static int cmd_test_xtimer_mutex_lock_timeout_long_unlocked(int argc,
                                                            char **argv);
static int cmd_test_xtimer_mutex_lock_timeout_long_locked(int argc,
                                                          char **argv);

/**
 * @brief   List of command for this application.
 */
static const shell_command_t shell_commands[] = {
    { "mutex_timeout_long_unlocked", "unlocked mutex with long timeout",
      cmd_test_xtimer_mutex_lock_timeout_long_unlocked, },
    { "mutex_timeout_long_locked", "locked mutex with long timeout",
      cmd_test_xtimer_mutex_lock_timeout_long_locked, },
    { NULL, NULL, NULL }
};

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout
 *
 * the mutex is not locked before the function call and
 * the timer long. Meaning the timer will get removed
 * before triggering.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_mutex_lock_timeout_long_unlocked(int argc,
                                                            char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: xtimer mutex lock timeout");
    mutex_t test_mutex = MUTEX_INIT;

    if (xtimer_mutex_lock_timeout(&test_mutex, LONG_MUTEX_TIMEOUT) == 0) {
        /* mutex has to be locked */
        if (mutex_trylock(&test_mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error mutex not locked");
        }
    }
    else {
        puts("error: mutex timed out");
    }

    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout
 *
 * the mutex is locked before the function call and
 * the timer long. Meaning the timer will trigger
 * and remove the thread from the mutex waiting list.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_mutex_lock_timeout_long_locked(int argc,
                                                          char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: xtimer mutex lock timeout");
    mutex_t test_mutex = MUTEX_INIT;
    mutex_lock(&test_mutex);

    if (xtimer_mutex_lock_timeout(&test_mutex, LONG_MUTEX_TIMEOUT) == 0) {
        puts("Error: mutex taken");
    }
    else {
        /* mutex has to be locked */
        if (mutex_trylock(&test_mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error mutex not locked");
        }
    }

    /* to make the test easier to read */
    printf("\n");

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
