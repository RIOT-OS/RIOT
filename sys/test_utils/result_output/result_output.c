/*
 * Copyright (C) 2021 HAW Hamburg
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
 * @brief       Result output common implementation
 *
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 * @}
 */

#include <stdint.h>
#include <stddef.h>

#include "test_utils/result_output.h"

void __attribute__((weak)) turo_init(turo_t *ctx)
{
    (void)ctx;
}

void turo_array_u8(turo_t *ctx, uint8_t *vals, size_t size)
{
    turo_array_open(ctx);
    while (size > 0) {
        turo_u32(ctx, (uint32_t)*vals);
        vals++;
        size--;
    }
    turo_array_close(ctx);
}

void turo_array_s32(turo_t *ctx, int32_t *vals, size_t size)
{
    turo_array_open(ctx);
    while (size-- > 0) {
        turo_s32(ctx, *vals);
        vals++;
    }
    turo_array_close(ctx);
}

void turo_dict_string(turo_t *ctx, const char *key, const char *val)
{
    turo_dict_open(ctx);
    turo_dict_key(ctx, key);
    turo_string(ctx, val);
    turo_dict_close(ctx);
}

void turo_dict_s32(turo_t *ctx, const char *key, int32_t val)
{
    turo_dict_open(ctx);
    turo_dict_key(ctx, key);
    turo_s32(ctx, val);
    turo_dict_close(ctx);
}

void turo_simple_s32(turo_t *ctx, int32_t val)
{
    turo_container_open(ctx);
    turo_s32(ctx, val);
    turo_container_close(ctx, 0);
}

void turo_simple_array_s32(turo_t *ctx, int32_t *vals, size_t size)
{
    turo_container_open(ctx);
    turo_array_s32(ctx, vals, size);
    turo_container_close(ctx, 0);
}

void turo_simple_array_u8(turo_t *ctx, uint8_t *vals, size_t size)
{
    turo_container_open(ctx);
    turo_array_u8(ctx, vals, size);
    turo_container_close(ctx, 0);
}

void turo_simple_dict_string(turo_t *ctx, const char *key, const char *val)
{
    turo_container_open(ctx);
    turo_dict_open(ctx);
    turo_dict_key(ctx, key);
    turo_string(ctx, val);
    turo_dict_close(ctx);
    turo_container_close(ctx, 0);
}

void turo_simple_dict_s32(turo_t *ctx, const char *key, int32_t val)
{
    turo_container_open(ctx);
    turo_dict_open(ctx);
    turo_dict_key(ctx, key);
    turo_s32(ctx, val);
    turo_dict_close(ctx);
    turo_container_close(ctx, 0);
}

void turo_simple_exit_status(turo_t *ctx, int exit_status)
{
    turo_container_open(ctx);
    turo_container_close(ctx, exit_status);
}
