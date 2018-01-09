/*
 * Copyright (C) 2016  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "tests-json.h"

#include <stdio.h>

#define TJPRAR_MAX_DEPTH 10

typedef enum {
    TJPRAR_Y,
    TJPRAR_N,
    TJPRAR_RECURSION_DEPTH,
    TJPRAR_FATAL,
} tjprar_result;

static const char *current_test;

static tjprar_result tests_json_parsing_read_all_recursive(
        tests_json_read_cookie_t *c,
        unsigned depth);

static tjprar_result tjprar_result_only_ok_inv_prem(json_result_t result) {
    switch (result) {
        case JSON_OKAY:
            return TJPRAR_Y;

        case JSON_INVALID_DATA:
        case JSON_PREMATURELY_ENDED:
            return TJPRAR_N;

        case JSON_READ_UNICODE:
        default:
            break;
    }

    addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
    return TJPRAR_FATAL;
}

static tjprar_result tests_json_parsing_read_char_result(
        tests_json_read_cookie_t *c,
        json_result_t read_result,
        size_t len,
        bool done) {
    switch (read_result) {
        case JSON_OKAY: {
            if (done) {
                return TJPRAR_Y;
            }
            else if (len != 1) {
                addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
                return TJPRAR_FATAL;
            }
            else {
                return TJPRAR_Y;
            }
        }
        case JSON_INVALID_DATA:
        case JSON_PREMATURELY_ENDED:
            return TJPRAR_N;
        case JSON_READ_UNICODE:
            break; /* handled below */
        default:
            addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
            return TJPRAR_FATAL;
    }

    /* read_result == JSON_READ_UNICODE */
    uint32_t codepoint = json_read_unicode(&c->cookie);
    if (codepoint > 0x0010FFFF) {
        addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
        return TJPRAR_FATAL;
    }

    char utf8_buffer[4];
    size_t utf_len;
    json_result_t convert_result = json_codepoint_to_utf8(
            codepoint, utf8_buffer, &utf_len);
    switch (convert_result) {
        case JSON_OKAY:
            if ((1 <= utf_len) && (utf_len <= 4)) {
                return TJPRAR_Y;
            }
            else {
                addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
                return TJPRAR_FATAL;
            }
        case JSON_INVALID_DATA:
            return TJPRAR_N;
        case JSON_PREMATURELY_ENDED:
        case JSON_READ_UNICODE:
        default:
            addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
            return TJPRAR_FATAL;
    }
}

static tjprar_result tests_json_parsing_read_object(
        tests_json_read_cookie_t *c,
        unsigned depth) {
    json_result_t read_result = json_read_object(&c->cookie);
    if (read_result != JSON_OKAY) {
        return tjprar_result_only_ok_inv_prem(read_result);
    }

    while (1) { /* read all items */
        bool read_key_loop_first_char = true;
        while (1) { /* read whole key */
            bool object_closed;
            char key_buffer[1];
            size_t key_len = 0;
            bool key_read;
            json_result_t read_result = json_read_object_next(
                    &c->cookie, &object_closed,
                    key_buffer, 1, &key_len, &key_read);

            if ((read_result == JSON_OKAY) && object_closed) {
                if (read_key_loop_first_char) {
                    return TJPRAR_Y;
                }
                else {
                    addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
                    return TJPRAR_FATAL;
                }
            }
            read_key_loop_first_char = false;

            tjprar_result char_result = tests_json_parsing_read_char_result(
                    c, read_result, key_len, key_read);
            if (char_result != TJPRAR_Y) {
                return char_result;
            }
            else if (key_read) {
                break;
            }
        }

        tjprar_result recursive_result = tests_json_parsing_read_all_recursive(c, depth);
        if (recursive_result != TJPRAR_Y) {
            return recursive_result;
        }
    }
}

static tjprar_result tests_json_parsing_read_array(
        tests_json_read_cookie_t *c,
        unsigned depth) {
    json_result_t read_result = json_read_array(&c->cookie);
    if (read_result != JSON_OKAY) {
        return tjprar_result_only_ok_inv_prem(read_result);
    }

    while (1) { /* read all fields */
        bool done;
        json_result_t next_result = json_read_array_next(&c->cookie, &done);
        if (next_result != JSON_OKAY) {
            return tjprar_result_only_ok_inv_prem(next_result);
        }
        else if (done) {
            return TJPRAR_Y;
        }

        tjprar_result recursive_result = tests_json_parsing_read_all_recursive(c, depth);
        if (recursive_result != TJPRAR_Y) {
            return recursive_result;
        }
    }
}

static tjprar_result tests_json_parsing_read_number(tests_json_read_cookie_t *c) {
    char buffer[100];
    size_t len = 0;
    bool done;
    json_result_t read_result = json_read_number(&c->cookie, buffer, sizeof(buffer), &len, &done);
    if (read_result != JSON_OKAY) {
        return tjprar_result_only_ok_inv_prem(read_result);
    }

    if (!done) {
        /* buffer size exceeded, this is not an error per se */
        return TJPRAR_N;
    }
    else if (len == 0) {
        addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
        return TJPRAR_FATAL;
    }

    json_number_t number_type;
    json_result_t type_result = json_number_type(buffer, len, &number_type);
    if (type_result != JSON_OKAY) {
        return tjprar_result_only_ok_inv_prem(type_result);
    }

    return TJPRAR_Y;
}

static tjprar_result tests_json_parsing_read_string(tests_json_read_cookie_t *c) {
    while (1) {
        char buffer[1];
        size_t len = 0;
        bool done;
        json_result_t read_result = json_read_string(&c->cookie, buffer, 1, &len, &done);

        tjprar_result char_result = tests_json_parsing_read_char_result(
                c, read_result, len, done);
        if (char_result != TJPRAR_Y) {
            return char_result;
        }
        else if (done) {
            return TJPRAR_Y;
        }
    }
}

static tjprar_result tests_json_parsing_read_all_recursive(
        tests_json_read_cookie_t *c,
        unsigned depth) {
    if (depth >= TJPRAR_MAX_DEPTH) {
        return TJPRAR_RECURSION_DEPTH;
    }

    json_type_t json_type;
    json_result_t peek_result = json_read_peek(&c->cookie, &json_type);
    switch (peek_result) {
        case JSON_OKAY:
            break;
        case JSON_INVALID_DATA:
        case JSON_PREMATURELY_ENDED:
            return TJPRAR_N;
        case JSON_READ_UNICODE:
        default:
            addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
            return TJPRAR_FATAL;
    }

    switch (json_type) {
        case JSON_OBJECT:
            return tests_json_parsing_read_object(c, depth + 1);
        case JSON_ARRAY:
            return tests_json_parsing_read_array(c, depth + 1);
        case JSON_NUMBER:
            return tests_json_parsing_read_number(c);
        case JSON_STRING:
            return tests_json_parsing_read_string(c);
        case JSON_TRUE: {
            json_result_t read_result = json_read_true(&c->cookie);
            return tjprar_result_only_ok_inv_prem(read_result);
        }
        case JSON_FALSE: {
            json_result_t read_result = json_read_false(&c->cookie);
            return tjprar_result_only_ok_inv_prem(read_result);
        }
        case JSON_NULL: {
            json_result_t read_result = json_read_null(&c->cookie);
            return tjprar_result_only_ok_inv_prem(read_result);
        }
        default:
            break;
    }

    addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
    return TJPRAR_FATAL;
}

void tests_json_parsing(void) {
    for (size_t i = 0; i < tests_json_parsing_tests_count; ++i) {
        current_test = tests_json_parsing_tests[i].name;

        tests_json_read_cookie_t c;
        tests_json_setup_cookie(&c, tests_json_parsing_tests[i].content, tests_json_parsing_tests[i].size);
        tjprar_result outcome = tests_json_parsing_read_all_recursive(&c, 0);

        /* msp430-gcc (v4.6.3) assumes that was_accepted may be used uninitialized */
        bool was_accepted = false;
        switch (outcome) {
            case TJPRAR_Y: {
                /* ensure that there is no more data in the stream */
                json_result_t spaces_result = json_read_read_spaces(&c.cookie);
                switch (spaces_result) {
                    case JSON_PREMATURELY_ENDED:
                        was_accepted = true;
                        break;
                    case JSON_INVALID_DATA:
                        was_accepted = false;
                        break;
                    case JSON_OKAY:
                    case JSON_READ_UNICODE:
                        addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
                        return;
                }
                break;
            }
            case TJPRAR_N:
            case TJPRAR_RECURSION_DEPTH: {
                was_accepted = false;
                break;
            }
            case TJPRAR_FATAL:
            default: {
                return;
            }
        }

        switch (current_test[0]) {
            case 'y': {
                if (was_accepted) {
                    continue;
                }
                addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
                return;
            }
            case 'n': {
                if (!was_accepted) {
                    continue;
                }
                addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
                return;
            }
            case 'i':
#if 0           /* enable if your are interested in the outcome of underspecified and edge cases */
                printf("%s: was_accepted = %s\n", current_test, was_accepted ? "yes" : "no");
#endif
                continue;

            default:
                addFailure(current_test, __LINE__, RIOT_FILE_RELATIVE);
                return;
        }
    }
}
