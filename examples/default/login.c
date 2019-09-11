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

#include "pbkdf2.h"

const char user[] = "admin";

#define PBKDF2_ITERS 1000
static const uint8_t salt[] = {0x70, 0x16, 0x2E, 0x1F, 0x2E, 0x38, 0x5, 0x8E,
                               0xF0, 0x6B, 0xB1, 0x3, 0xCE, 0xE3, 0x6E, 0x73,
                               0x3D, 0x28, 0x5C, 0xEE, 0xDA, 0x15, 0xB0, 0x5B,
                               0x3F, 0xF7, 0x67, 0xB6, 0x24, 0xD0, 0xBE, 0x47,
                               0x57, 0x2E, 0x43, 0xAA, 0x74, 0xC8, 0xF1, 0x7F,
                               0x55, 0x3B, 0x2F, 0xA4, 0xE2, 0x8F, 0xAD, 0x4D,
                               0x28, 0x63, 0x27, 0x3, 0x8B, 0xDF, 0x11, 0x23,
                               0xB3, 0x4B, 0x97, 0x74, 0x1C, 0x4E, 0xB3, 0x62};
static const uint8_t key[] = {0xD7, 0xEE, 0xF0, 0xCA, 0xD5, 0x29, 0x92, 0xE1,
                              0xDB, 0x40, 0xE0, 0x2B, 0xD5, 0xFC, 0xD1, 0x84,
                              0xD5, 0xD2, 0xDB, 0x26, 0xA9, 0xFE, 0x45, 0x64,
                              0x4B, 0x6C, 0x9, 0x26, 0xB0, 0x56, 0xCD, 0x47};

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
    assert(PBKDF2_KEY_SIZE >= sizeof(key));

    fputs("Username: ", stdout);
    fflush(stdout);

    read_len = gets_echoing(line_buf, buf_size, 0);

    putchar('\r');
    putchar('\n');

    if (read_len == -LINE_CANCELLED) {
        goto login_end;
    } else if (read_len > 0) {
        state += !!crypto_equals((uint8_t*)line_buf, (uint8_t*)user, sizeof(user));
    }

    fputs("Password: ", stdout);
    fflush(stdout);

    read_len = gets_echoing(line_buf, buf_size, '*');

    putchar('\r');
    putchar('\n');

    if (read_len == -LINE_CANCELLED) {
        goto login_end;
    } else if (read_len > 0) {
        uint8_t this_key[PBKDF2_KEY_SIZE];

        pbkdf2_sha256((uint8_t*)line_buf, read_len-1,
                      salt, sizeof(salt),
                      PBKDF2_ITERS,
                      this_key);

        state += !!crypto_equals(this_key, key, sizeof(key));
    }

login_end:
    crypto_secure_wipe(line_buf, buf_size);

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
