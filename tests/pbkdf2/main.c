/*
 * Copyright (C) 2019 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Test PBKDF2-sha256 implementation.
 *
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 *
 * This application reads (password, salt, iterations) tuples from the
 * standard input and outputs the derived key.
 *
 * The salt must be base64 encoded. The key is printed as base64.
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "base64.h"
#include "hashes/pbkdf2.h"

const char error_message[] = "{error}";
const char input_message[] = "{ready}";

#define LINEBUF_SZ (128)

enum TEST_STATE {
    TEST_READ_PASS,
    TEST_READ_SALT,
    TEST_READ_ITERS,
    TEST_COMPUTE,
    TEST_ERROR
};

static void _clear_input(void)
{
    /* clear input buffer */
    int c;
    while ( (c = getchar()) != '\n' && c != EOF ) { }
}

int main(void)
{
    static char linebuf[LINEBUF_SZ];

    /* There will be a few bytes wasted here */
    static char password[LINEBUF_SZ];
    static uint8_t salt[LINEBUF_SZ];
    static uint8_t key[PBKDF2_KEY_SIZE];

    size_t passwd_len = 0, salt_len = 0;
    int iterations = 0;

    enum TEST_STATE state = TEST_READ_PASS;

    _clear_input();

    while ((puts(input_message), fgets(linebuf, LINEBUF_SZ, stdin) != NULL)) {
        char *s_end;
        int conversion_status, line_len = strlen(linebuf)-1;
        size_t b64_buff_size;

        linebuf[line_len] = '\0';

        switch (state) {
            case TEST_READ_PASS:
                strcpy(password, linebuf);
                passwd_len = line_len;
                state++;

                break;
            case TEST_READ_SALT:
                /* work around bug in base64_decode */
                if (line_len == 0) {
                    salt_len = 0;
                    conversion_status = BASE64_SUCCESS;
                } else {
                    salt_len = sizeof(salt);
                    conversion_status = base64_decode((uint8_t*)linebuf,
                                                      line_len+1,
                                                      salt, &salt_len);
                }

                if (conversion_status == BASE64_SUCCESS) {
                    state++;
                } else {
                    state = TEST_ERROR;
                }

                break;
            case TEST_READ_ITERS:
                iterations = strtol(linebuf, &s_end, 10);

                if (*s_end != '\0') {
                    state = TEST_ERROR;
                } else {
                    state++;
                }

                break;
            default:
                assert(1);
                break;
        }

        switch (state) {
            case TEST_COMPUTE:
                pbkdf2_sha256((uint8_t*)password, passwd_len, salt, salt_len,
                              iterations, key);

                b64_buff_size = sizeof(linebuf);
                conversion_status = base64_encode(key, sizeof(key),
                                                  (uint8_t*)linebuf,
                                                  &b64_buff_size);

                if (conversion_status == BASE64_SUCCESS) {
                    linebuf[b64_buff_size] = 0;
                    puts(linebuf);
                } else {
                    puts(error_message);
                }

                state = TEST_READ_PASS;
                break;
            case TEST_ERROR:
                puts(error_message);
                state = TEST_READ_PASS;
                break;
            default:
                break;
        }
    }

    return 0;
}
