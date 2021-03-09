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
 * This module also provides a pseudomodule for automated locking after a given
 * interval. Add "USEMODULE += shell_lock_auto_locking" to your Makefile to
 * enable this feature.
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

static bool _shell_is_locked = true;

#ifdef MODULE_SHELL_LOCK_AUTO_LOCKING
static xtimer_t _shell_auto_lock_xtimer;
#endif

/* defined in shell.c */
extern void flush_if_needed(void);

/* defined in shell.c */
extern void print_prompt(void);

/* defined in shell.c */
extern int readline(char *buf, size_t size);

int _lock_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    _shell_is_locked = true;

    return 0;
}

/* This should be independent from the module "shell_commands", as the user may
 * don't want any other command than the mandatory "lock"-command. */
const shell_command_t _shell_lock_command_list[] = {
        {"lock", "Lock the shell", _lock_handler},
        {NULL, NULL, NULL}
};

/* Implementation of strcmp that does not return after the first difference
 * which could give away information about the first n correct characters of
 * the password. The length of the loop is only dependent on the input string.
 * Don't optimize this function by a compiler. */
static bool __attribute__((optimize("O0"))) _safe_strcmp(char* input, char* pwd)
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
    int success = false;

    puts("Password:");

    print_prompt();

    if (readline(line_buf, buf_size) > 0) {

        if (_safe_strcmp(line_buf, SHELL_LOCK_PASSWORD)) {
            success = true;
        }
    }

    return success;
}

/**
 * Repeatedly prompt for the password.
 *
 * This function won't return until the correct password has been entered.
 */
void _login_barrier(char *line_buf, size_t buf_size)
{
    while (1) {
        int attempts = ATTEMPTS_BEFORE_TIME_LOCK;

        while (attempts--) {
            if (_login(line_buf, buf_size)) {
                return;
            }
            puts("Wrong password");
            xtimer_sleep(1);
        }
        xtimer_sleep(7);
    }
}

#ifdef MODULE_SHELL_LOCK_AUTO_LOCKING
void _shell_auto_lock_xtimer_callback(void *arg)
{
    (void) arg;

    _shell_is_locked = true;
}

void _refresh_shell_auto_lock(void)
{
    xtimer_remove(&_shell_auto_lock_xtimer);
    xtimer_set(&_shell_auto_lock_xtimer,
               (SHELL_LOCK_AUTO_LOCK_TIMEOUT_MS) * US_PER_MS);
}

void shell_lock_auto_lock_refresh(void)
{
    _refresh_shell_auto_lock();
}
#endif

bool shell_lock_is_locked(void)
{
    return _shell_is_locked;
}

void shell_lock_checkpoint(char *line_buf, int buf_size)
{
    if (_shell_is_locked) {
        printf("The shell is locked. Enter a valid password to unlock.\n\n");

        _login_barrier(line_buf, buf_size);

        if (IS_USED(MODULE_SHELL_LOCK_AUTO_LOCKING)) {
            printf("Shell was unlocked.\n\n");
        }
        else {
            printf("Shell was unlocked.\n\n"
                   "IMPORTANT: Don't forget to lock the shell after usage, "
                   "because it won't lock itself.\n\n");
        }

        _shell_is_locked = false;
    }

    #ifdef MODULE_SHELL_LOCK_AUTO_LOCKING
    _shell_auto_lock_xtimer.callback = &_shell_auto_lock_xtimer_callback;
    _refresh_shell_auto_lock();
    #endif
}
