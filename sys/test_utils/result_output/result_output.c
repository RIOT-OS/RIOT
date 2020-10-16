/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Result output implementation
 *
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */

#include <stdint.h>
#include <stdbool.h>

#include "assert.h"
#include "fmt.h"

#include "test_utils/result_output.h"

#define _DATA_STR   "data"

void turo_data_int(int turo, int32_t data)
{
    (void)turo;
    print_str("{ \""_DATA_STR"\": ");
    print_s32_dec(data);
    print_str(" }\n");
}

void turo_data_bool(int turo, bool data)
{
    (void)turo;
    print_str("{ \""_DATA_STR"\": ");
    if (data) {
        print_str("True  }\n");
    }
    else {
        print_str("False  }\n");
    }
}

void turo_data_string(int turo, char* data)
{
    (void)turo;
    print_str("{ \""_DATA_STR"\": \"");
    print_str(data);
    print_str("\" }\n");
}

void turo_data_u8_array(int turo, uint8_t* data, size_t size)
{
    (void)turo;
    assert(size > 0);
    print_str("{ \"data\": [ ");
    for (size_t i = 0; i < size; i++) {
        print_s32_dec((int32_t)data[i]);
        if (i < size - 1) {
            print_str(", ");
        }
    }
    print_str(" ] }\n");
}

void turo_data_i32_array(int turo, int32_t* data, size_t size)
{
    (void)turo;
    assert(size > 0);
    print_str("{ \""_DATA_STR"\": [ ");
    for (size_t i = 0; i < size; i++) {
        print_s32_dec(data[i]);
        if (i < size - 1) {
            print_str(", ");
        }
    }
    print_str(" ] }\n");
}

void turo_data_string_dict(int turo, char* key, char* value)
{
    (void)turo;
    print_str("{ \""_DATA_STR"\": { \"");
    print_str(key);
    print_str("\": \"");
    print_str(value);
    print_str("\" } }\n");
}

void turo_data_int_dict(int turo, char* key, int32_t value)
{
    (void)turo;
    print_str("{ \""_DATA_STR"\": { \"");
    print_str(key);
    print_str("\": ");
    print_s32_dec(value);
    print_str(" } }\n");
}

void turo_result_success(int turo)
{
    (void)turo;
    print_str("{ \"result\": \"Success\" }\n");
}

void turo_result_error(int turo)
{
    (void)turo;
    print_str("{ \"result\": \"Error\" }\n");
}

void turo_result_error_with_code(int turo, int error)
{
    print_str("{ \"error_code\": ");
    print_s32_dec((int32_t)error);
    print_str(" }\n");
    turo_result_error(turo);
}
