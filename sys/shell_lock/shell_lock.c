/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_lock
 * @{
 *
 * @file
 * @brief       Module to lock the running shell with a password.
 *
 * The Shell is proceeded only when the valid password was entered by the user.
 * After 3 (default) failed attempts, the input is blocked for a few seconds to
 * slow down brute force attacks.
 * Does not make use of any cryptographic features yet.
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "xtimer.h"

#include "shell_lock.h"

#if defined(MODULE_NEWLIB) || defined(MODULE_PICOLIBC)
    #define flush_if_needed() fflush(stdout)
#else
    #define flush_if_needed()
#endif /* MODULE_NEWLIB || MODULE_PICOLIBC */

static bool _shell_is_locked = true;

/* defined in shell.c */
extern int readline(char *buf, size_t size);

static int _lock_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    _shell_is_locked = true;

    return 0;
}

SHELL_COMMAND(lock, "Lock the shell", _lock_handler);

static inline void _print_password_prompt(void)
{
    printf("Password: ");
    flush_if_needed();
}

/* Implementation of strcmp that does not return after the first difference
 * which could give away information about the first n correct characters of
 * the password. The length of the loop is only dependent on the input string.
 * Don't optimize this function by a compiler. */
static bool __attribute__((optimize("O0"))) _safe_strcmp(const char* input, const char* pwd)
{
    bool the_same = true;

    int input_len = strlen(input);
    int pwd_len = strlen(pwd);

    int input_index = 0;
    int pwd_index = 0;

    do {
        if (input[input_index] != pwd[pwd_index]) {
            the_same &= false;
        }
        else {
            the_same &= true;
        }

        /* keep indices at last index of respective string */
        if (input_index < input_len) {
            input_index++;
        }

        if (pwd_index < pwd_len) {
            pwd_index++;
        }

    } while (input[input_index] != '\0');

    if (input_len != pwd_len) {
        /* substring of the password doesn't count */
        return false;
    }

    return the_same;
}

static bool _login(char *line_buf, size_t buf_size)
{
    _print_password_prompt();

    if (readline(line_buf, buf_size) > 0) {
        return _safe_strcmp(line_buf, CONFIG_SHELL_LOCK_PASSWORD);
    }

    return false;
}

/**
 * Repeatedly prompt for the password.
 *
 * This function won't return until the correct password has been entered.
 */
static void _login_barrier(char *line_buf, size_t buf_size)
{
    while (1) {
        int attempts = CONFIG_SHELL_LOCK_ATTEMPTS_BEFORE_TIME_LOCK;

        while (attempts--) {
            if (_login(line_buf, buf_size)) {
                return;
            }
            puts("Wrong password");
            ztimer_sleep(ZTIMER_MSEC, 1000);
        }
        ztimer_sleep(ZTIMER_MSEC, 7000);
    }
}

bool shell_lock_is_locked(void)
{
    return _shell_is_locked;
}

void shell_lock_checkpoint(char *line_buf, int buf_size)
{
    if (_shell_is_locked) {
        printf("The shell is locked. Enter a valid password to unlock.\n\n");

        _login_barrier(line_buf, buf_size);

        _shell_is_locked = false;
    }
}
