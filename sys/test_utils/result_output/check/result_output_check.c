/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "test_utils/result_output.h"

void turo_init(turo_t *ctx)
{
    ctx->idx = 0;
}

void turo_container_open(turo_t *ctx)
{
    assert(ctx->idx == 0);
    assert(ctx->idx < CONFIG_TURO_MAX_NESTING_LEVELS);
    ctx->states[ctx->idx++] = TURO_STATE_CONTAINER;
    ctx->states[ctx->idx] = TURO_STATE_READY;
}

static void _val_check(turo_t *ctx)
{
    assert(ctx->idx > 0);
    assert(ctx->states[ctx->idx] != TURO_STATE_DICT_OPENED);
    ctx->states[ctx->idx] = TURO_STATE_READY;
}

void turo_s32(turo_t *ctx, int32_t val)
{
    (void)val;
    _val_check(ctx);
}

void turo_u32(turo_t *ctx, uint32_t val)
{
    (void)val;
    _val_check(ctx);
}

void turo_s64(turo_t *ctx, int64_t val)
{
    (void)val;
    _val_check(ctx);
}

void turo_u64(turo_t *ctx, uint64_t val)
{
    (void)val;
    _val_check(ctx);
}

void turo_float(turo_t *ctx, float val)
{
    (void)val;
    _val_check(ctx);
}

void turo_string(turo_t *ctx, const char *str)
{
    (void)str;
    _val_check(ctx);
}

void turo_bool(turo_t *ctx, bool val)
{
    (void)val;
    _val_check(ctx);
}

void turo_dict_open(turo_t *ctx)
{
    assert(ctx->idx > 0);
    assert(ctx->idx < CONFIG_TURO_MAX_NESTING_LEVELS);
    ctx->states[ctx->idx++] = TURO_STATE_DICT_OPENED;
    ctx->states[ctx->idx] = TURO_STATE_DICT_OPENED;
}

void turo_dict_key(turo_t *ctx, const char *key)
{
    (void)key;
    assert(ctx->idx > 0);
    assert(ctx->states[ctx->idx - 1] == TURO_STATE_DICT_OPENED);
    assert(ctx->states[ctx->idx] == TURO_STATE_READY ||
           ctx->states[ctx->idx] == TURO_STATE_DICT_OPENED);
    ctx->states[ctx->idx] = TURO_STATE_READY;
}

void turo_dict_close(turo_t *ctx)
{
    assert(ctx->idx > 0);
    assert(ctx->states[ctx->idx] == TURO_STATE_READY);
    assert(ctx->states[--ctx->idx] == TURO_STATE_DICT_OPENED);
    ctx->states[ctx->idx] = TURO_STATE_READY;
}

void turo_array_open(turo_t *ctx)
{
    assert(ctx->idx > 0);
    assert(ctx->idx < CONFIG_TURO_MAX_NESTING_LEVELS);
    ctx->states[ctx->idx++] = TURO_STATE_ARRAY_OPENED;
    ctx->states[ctx->idx] = TURO_STATE_READY;
}

void turo_array_close(turo_t *ctx)
{
    assert(ctx->idx > 0);
    assert(ctx->states[ctx->idx] == TURO_STATE_READY);
    assert(ctx->states[--ctx->idx] == TURO_STATE_ARRAY_OPENED);
    ctx->states[ctx->idx] = TURO_STATE_READY;
}

void turo_container_close(turo_t *ctx, int exit_status)
{
    (void)exit_status;
    assert(ctx->idx == 1);
    assert(ctx->states[ctx->idx] == TURO_STATE_READY);
    assert(ctx->states[0] == TURO_STATE_CONTAINER);
    ctx->idx--;
}
