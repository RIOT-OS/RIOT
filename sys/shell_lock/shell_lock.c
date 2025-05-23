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

#ifdef MODULE_STDIO_TELNET
#include "net/telnet.h"
#endif
#include "ztimer.h"

#include "shell_lock.h"

#if defined(MODULE_NEWLIB) || defined(MODULE_PICOLIBC)
    #define flush_if_needed() fflush(stdout)
#else
    #define flush_if_needed()
#endif /* MODULE_NEWLIB || MODULE_PICOLIBC */

static bool _shell_is_locked = true;

#ifdef MODULE_SHELL_LOCK_AUTO_LOCKING
static ztimer_t _shell_auto_lock_ztimer;
#endif

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
static bool _safe_strcmp(const char* input, const char* pwd)
{
    volatile bool the_same = true;

    int input_index = 0;
    int pwd_index = 0;

    do {
        the_same &= input[input_index] == pwd[pwd_index];

        /* keep indices at last index of respective string */
        input_index += input[input_index] != '\0';
        pwd_index += pwd[pwd_index] != '\0';

    } while (input[input_index] != '\0' );

    /* ensure last char is the same */
    the_same &= input[input_index] == pwd[pwd_index];

    /* ensure last index is the same */
    the_same &= input_index == pwd_index;

    return the_same;
}

static bool _login(char *line_buf, size_t buf_size)
{
    _print_password_prompt();

    while (1) {
        memset(line_buf, 0, buf_size);
        while (shell_readline(line_buf, buf_size) > 0) {
            return _safe_strcmp(line_buf, CONFIG_SHELL_LOCK_PASSWORD);
        }
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
#ifdef MODULE_STDIO_TELNET
        telnet_server_disconnect();
#endif
        ztimer_sleep(ZTIMER_MSEC, 7000);
    }
}

#ifdef MODULE_STDIO_TELNET
void telnet_cb_disconneced(void)
{
    shell_lock_do_lock();
}
#endif

void shell_lock_do_lock(void)
{
    _shell_is_locked = true;
}

#ifdef MODULE_SHELL_LOCK_AUTO_LOCKING
static void _shell_auto_lock_ztimer_callback(void *arg)
{
    (void) arg;

#ifdef MODULE_STDIO_TELNET
    telnet_server_disconnect();
#endif
    _shell_is_locked = true;
}

void shell_lock_auto_lock_refresh(void)
{
    ztimer_remove(ZTIMER_MSEC, &_shell_auto_lock_ztimer);
    ztimer_set(ZTIMER_MSEC, &_shell_auto_lock_ztimer,
               CONFIG_SHELL_LOCK_AUTO_LOCK_TIMEOUT_MS);
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
    _shell_auto_lock_ztimer.callback = &_shell_auto_lock_ztimer_callback;
    shell_lock_auto_lock_refresh();
#endif
}
