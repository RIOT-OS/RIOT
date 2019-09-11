/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief       Proof of concept of user/password protection for the serial
 *              interface (shell).
 *
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdbool.h>

#include "xtimer.h"

const char user[] = "admin";
const char pass[] = "Passw0rd!";

static bool is_line_delim(char c)
{
    return c == '\r' || c == '\n';
}

static bool is_line_cancel(char c)
{
    return c == 0x03 || c == 0x04;
}

static bool input_compare(const char *target, char mask_char)
{
    bool login_ok = true;
    char cmp_this;

    do {
        int c = getchar();

        if (c == EOF || is_line_cancel(c)) {
            cmp_this = '\0';
            login_ok = false;
        } else if (is_line_delim(c)) {
            cmp_this = '\0';
        } else {
            cmp_this = c;
            putchar(mask_char? mask_char: c);
            fflush(stdout);
        }

        if (*target != cmp_this) {
            login_ok = false;
        }

        if (*target != '\0') {
            target++;
        }
    } while (cmp_this != '\0');

    return login_ok;
}

enum LOGIN_STATE {
    LOGIN_WRONG,
    LOGIN_OK_ONE,
    LOGIN_OK_BOTH,
};

static bool login(void)
{
    int state = LOGIN_WRONG;

    fputs("Username: ", stdout);
    fflush(stdout);

    state += !!input_compare(user, 0);

    fputs("\r\nPassword: ", stdout);
    fflush(stdout);

    state += !!input_compare(pass, '*');

    putchar('\r');
    putchar('\n');

    return state == LOGIN_OK_BOTH;
}

#define N_ATTEMPTS 3

/**
 * Repeatedly prompt for the user and password.
 *
 * This function won't return until the correct user-pass pair has been
 * introduced.
 */
void secure_login(void)
{
    while (1) {
        int attempts = N_ATTEMPTS;

        while (attempts--) {
            if (login()) {
                return;
            }
            puts("Wrong user/pass");
            xtimer_sleep(1);
        }
        xtimer_sleep(7);
    }
}
