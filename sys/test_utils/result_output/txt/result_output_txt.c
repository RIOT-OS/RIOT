/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdbool.h>

#include "fmt.h"

#include "test_utils/result_output.h"

void turo_container_open(turo_t *ctx)
{
    (void)ctx;
}

void turo_s32(turo_t *ctx, int32_t val)
{
    (void)ctx;
    print_s32_dec(val);
    print_str(" ");
}

void turo_u32(turo_t *ctx, uint32_t val)
{
    (void)ctx;
    print_u32_dec(val);
    print_str(" ");
}

void turo_s64(turo_t *ctx, int64_t val)
{
    (void)ctx;
    print_s64_dec(val);
    print_str(" ");
}

void turo_u64(turo_t *ctx, uint64_t val)
{
    (void)ctx;
    print_u64_dec(val);
    print_str(" ");
}

void turo_float(turo_t *ctx, float val)
{
    (void)ctx;
    print_float(val, 7);
    print_str(" ");
}

void turo_string(turo_t *ctx, const char *str)
{
    (void)ctx;
    print_str(str);
    print_str(" ");
}

void turo_bool(turo_t *ctx, bool val)
{
    (void)ctx;
    if (val) {
        print_str("True");
    }
    else {
        print_str("False");
    }
    print_str(" ");
}

void turo_dict_open(turo_t *ctx)
{
    (void)ctx;
}

void turo_dict_key(turo_t *ctx, const char *key)
{
    (void)ctx;
    print_str(key);
    print_str(": ");

}

void turo_dict_close(turo_t *ctx)
{
    (void)ctx;
    print_str("\n");
}

void turo_array_open(turo_t *ctx)
{
    (void)ctx;
}

void turo_array_close(turo_t *ctx)
{
    (void)ctx;
    print_str("\n");
}

void turo_container_close(turo_t *ctx, int exit_status)
{
    (void)ctx;
    (void)exit_status;
    if (exit_status) {
        print_str("\nError\n");
    }
    else {
        print_str("\nSuccess\n");
    }
}
