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

static void _print_comma(turo_t *ctx, turo_state_t state)
{
    if (ctx->state == TURO_STATE_NEED_COMMA) {
        print_str(",");
    }
    ctx->state = state;
}

void turo_init(turo_t *ctx)
{
    ctx->state = TURO_STATE_READY;
}

void turo_container_open(turo_t *ctx)
{
    (void)ctx;
    print_str("[");
}

void turo_s32(turo_t *ctx, int32_t val)
{
    _print_comma(ctx, TURO_STATE_NEED_COMMA);
    print_s32_dec(val);
}

void turo_u32(turo_t *ctx, uint32_t val)
{
    _print_comma(ctx, TURO_STATE_NEED_COMMA);
    print_u32_dec(val);
}

void turo_s64(turo_t *ctx, int64_t val)
{
    _print_comma(ctx, TURO_STATE_NEED_COMMA);
    print_s64_dec(val);
}

void turo_u64(turo_t *ctx, uint64_t val)
{
    _print_comma(ctx, TURO_STATE_NEED_COMMA);
    print_u64_dec(val);
}

void turo_float(turo_t *ctx, float val)
{
    _print_comma(ctx, TURO_STATE_NEED_COMMA);
    print_float(val, 8);
}

void turo_string(turo_t *ctx, const char *str)
{
    _print_comma(ctx, TURO_STATE_NEED_COMMA);
    print_str("\"");
    print_str(str);
    print_str("\"");
}

void turo_bool(turo_t *ctx, bool val)
{
    _print_comma(ctx, TURO_STATE_NEED_COMMA);
    if (val) {
        print_str("true");
    }
    else {
        print_str("false");
    }
}

void turo_dict_open(turo_t *ctx)
{
    _print_comma(ctx, TURO_STATE_READY);
    print_str("{");
}

void turo_dict_key(turo_t *ctx, const char *key)
{
    _print_comma(ctx, TURO_STATE_READY);
    print_str("\"");
    print_str(key);
    print_str("\": ");
}

void turo_dict_close(turo_t *ctx)
{
    ctx->state = TURO_STATE_NEED_COMMA;
    print_str("}");
}

void turo_array_open(turo_t *ctx)
{
    _print_comma(ctx, TURO_STATE_READY);
    print_str("[");
}

void turo_array_close(turo_t *ctx)
{
    ctx->state = TURO_STATE_NEED_COMMA;
    print_str("]");
}

void turo_container_close(turo_t *ctx, int exit_status)
{
    _print_comma(ctx, TURO_STATE_READY);
    print_str("{\"exit_status\": ");
    print_s32_dec((int32_t)exit_status);
    print_str("}]\n");
}
