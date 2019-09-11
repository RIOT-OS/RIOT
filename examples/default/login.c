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

#include "crypto/helper.h"
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

enum LINE_TAINT {
    LINE_OK = 0,
    LINE_LONG = 1,
    LINE_OK_DONE = 2,
    LINE_LONG_DONE = 3,
    LINE_CANCELLED = 4,
};

#define MARK_DONE(t) ((t) | LINE_OK_DONE)
#define IS_DONE(t) ((t) & LINE_OK_DONE)

/**
 * Get a line of input, while echoing it back.
 *
 * The line is read to line_buf. If mask_char is not zero, then tht character
 * is echoed instead of the one inputted. EOF, ctrl-c, ctrl-d cancel the input
 * and (-LINE_CANCELLED) is returned.
 * Otherwise, return the number of characters read or, if the buffer size was
 * exceeded, (-LINE_LONG).
 *
 * At most buf_size-1 chracters will be stored, and the last character will
 * always be a terminator.
 *
 * Even if the buffer size is exceeded, characters will continue to be read
 * from the input.
 */
static int gets_echoing(char *line_buf, size_t buf_size, char mask_char)
{
    size_t length = 0;
    enum LINE_TAINT state = LINE_OK;

    do {
        int c = getchar();

        if (c == EOF || is_line_cancel(c)) {
            state = LINE_CANCELLED;
        } else if (is_line_delim(c)) {
            state = MARK_DONE(state);
        } else {
            if (length + 1 < buf_size) {
                line_buf[length++] = c;
            } else {
                state = LINE_LONG;
            }
            putchar(mask_char? mask_char: c);
            fflush(stdout);
        }
    } while (state != LINE_CANCELLED && !IS_DONE(state));

    line_buf[length++] = '\0';

    return state == LINE_OK_DONE? (int)length : -state;
}

enum LOGIN_STATE {
    LOGIN_WRONG,
    LOGIN_OK_ONE,
    LOGIN_OK_BOTH,
};

static bool login(char *line_buf, size_t buf_size)
{
    int read_len;
    int state = LOGIN_WRONG;

    assert(buf_size >= sizeof(user));
    assert(buf_size >= sizeof(pass));

    fputs("Username: ", stdout);
    fflush(stdout);

    read_len = gets_echoing(line_buf, buf_size, 0);

    if (read_len == -LINE_CANCELLED) {
        goto login_end;
    } else if (read_len > 0) {
        state += !!crypto_equals((uint8_t*)line_buf, (uint8_t*)user, sizeof(user));
    }

    fputs("\r\nPassword: ", stdout);
    fflush(stdout);

    read_len = gets_echoing(line_buf, buf_size, '*');

    if (read_len == -LINE_CANCELLED) {
        goto login_end;
    } else if (read_len > 0) {
        state += !!crypto_equals((uint8_t*)line_buf, (uint8_t*)pass, sizeof(pass));
    }

login_end:
    crypto_secure_wipe(line_buf, buf_size);

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
void secure_login(char *line_buf, size_t buf_size)
{
    while (1) {
        int attempts = N_ATTEMPTS;

        while (attempts--) {
            if (login(line_buf, buf_size)) {
                return;
            }
            puts("Wrong user/pass");
        }
        xtimer_sleep(7);
    }
}
