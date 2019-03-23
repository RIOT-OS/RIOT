/**
 * Print thread information.
 *
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_test_helper
 * @{
 * @file    test_helper.c
 * @brief   Helpers that can be used to write tests
 * @author  Kevin Weiss <kevin.weiss@haw-hamburg.de>
 * @}
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "test_helper.h"
#include "fmt.h"

void print_echo(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        print_str(argv[i]);
        print_str(" ");
    }
    print_str("\n");
}

void print_json_str(const char* name, const char* val)
{
    print("{\"", sizeof("{\""));
    print(name, fmt_strlen(name));
    print("\":\"", sizeof("\":\""));
    print(val, fmt_strlen(val));
    print("\"}\n", sizeof("\"}\n"));
}

void print_json_i32_dec(const char* name, int32_t val)
{
    print("{\"", sizeof("{\""));
    print(name, fmt_strlen(name));
    print("\":", sizeof("\":"));
    print_s32_dec(val);
    print("}\n", sizeof("}\n"));
}



void print_json_byte_array(const char* name, uint8_t *bytes, size_t size)
{
    print("{\"", sizeof("{\""));
    print(name, fmt_strlen(name));
    print("\":[", sizeof("\"["));
    while (size) {
        print_u32_dec(*(bytes));
        bytes++;
        if (size != 1) {
            print(",", sizeof(","));
        }
        size--;
    }
    print("]}\n", sizeof("]}\n"));
}
