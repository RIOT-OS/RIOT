/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Freie Universität Berlin
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer_convert_muldiv64
 * @{
 *
 * @file
 * @brief       ztimer frequency conversion module using 64bit division
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "ztimer/convert.h"
#include "ztimer/convert_muldiv64.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _ztimer_convert_muldiv64_set(ztimer_clock_t *ztimer, uint32_t val);

/* returns ceil(x/y) */
static uint64_t _integer_div_ceil(uint64_t x, uint64_t y)
{
    if (x == 0) {
        return 0;
    }

    return 1 + ((x - 1) / y);
}

static uint32_t _convert_muldiv64_set(
    const ztimer_convert_muldiv64_t *ztimer_convert_muldiv64, uint32_t val)
{
    uint64_t res = val;

    if (ztimer_convert_muldiv64->mul > 1) {
        res *= ztimer_convert_muldiv64->mul;
    }
    if (ztimer_convert_muldiv64->div > 1) {
        res = _integer_div_ceil(res, ztimer_convert_muldiv64->div);
    }

    return res;
}

static uint32_t _convert_muldiv64_now(
    const ztimer_convert_muldiv64_t *ztimer_convert_muldiv64, uint32_t val)
{
    uint64_t res = val;

    if (ztimer_convert_muldiv64->div > 1) {
        res *= ztimer_convert_muldiv64->div;
    }
    if (ztimer_convert_muldiv64->mul > 1) {
        res /= ztimer_convert_muldiv64->mul;
    }
    DEBUG("_convert_muldiv64_now(%u * %u / %u == %u\n", (unsigned)val,
          (unsigned)ztimer_convert_muldiv64->div ? ztimer_convert_muldiv64->div : 1,
          (unsigned)ztimer_convert_muldiv64->mul ? ztimer_convert_muldiv64->mul : 1,
          (unsigned)res);
    return res;
}

static void _ztimer_convert_muldiv64_set(ztimer_clock_t *ztimer, uint32_t val)
{
    ztimer_convert_muldiv64_t *ztimer_convert_muldiv64 =
        (ztimer_convert_muldiv64_t *)ztimer;

    ztimer_set(ztimer_convert_muldiv64->super.lower,
               &ztimer_convert_muldiv64->super.lower_entry, _convert_muldiv64_set(
                   ztimer_convert_muldiv64,
                   val));
}

static uint32_t _ztimer_convert_muldiv64_now(ztimer_clock_t *ztimer)
{
    const ztimer_convert_muldiv64_t *ztimer_convert_muldiv64 =
        (ztimer_convert_muldiv64_t *)ztimer;

    return _convert_muldiv64_now(ztimer_convert_muldiv64,
                                 ztimer_now(
                                     ztimer_convert_muldiv64->super.lower));
}

static const ztimer_ops_t _ztimer_convert_muldiv64_ops = {
    .set = _ztimer_convert_muldiv64_set,
    .now = _ztimer_convert_muldiv64_now,
    .cancel = ztimer_convert_cancel,
};

void ztimer_convert_muldiv64_init(
    ztimer_convert_muldiv64_t *ztimer_convert_muldiv64, ztimer_clock_t *lower,
    unsigned div, unsigned mul)
{
    uint32_t max_value;

    if (mul > div) {
        max_value = (uint64_t)UINT32_MAX * div / mul;
    }
    else {
        max_value = UINT32_MAX;
    }

    DEBUG(
        "ztimer_convert_muldiv64_init() div=%u mul=%u lower_maxval=%" PRIu32 "\n",
        div, mul, max_value);

    ztimer_convert_init(&ztimer_convert_muldiv64->super, lower, max_value);
    ztimer_convert_muldiv64->super.super.ops = &_ztimer_convert_muldiv64_ops;
    ztimer_convert_muldiv64->div = div;
    ztimer_convert_muldiv64->mul = mul;
    ztimer_init_extend(&ztimer_convert_muldiv64->super.super);
}
