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
 * @ingroup     sys_ztimer_convert_shift
 * @{
 *
 * @file
 * @brief       ztimer frequency conversion module using shifts
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "ztimer/convert.h"
#include "ztimer/convert_shift.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static uint32_t _convert_shift_up_set(
    ztimer_convert_shift_t *ztimer_convert_shift, uint32_t val)
{
    uint32_t res = val;

    res >>= ztimer_convert_shift->shift;

    /* TODO: round up */

    return res;
}

static uint32_t _convert_shift_up_now(
    ztimer_convert_shift_t *ztimer_convert_shift, uint32_t val)
{
    uint32_t res = val;

    res <<= ztimer_convert_shift->shift;

    DEBUG("_convert_shift_now(%u << %i == %u)\n", (unsigned)val,
          (unsigned)ztimer_convert_shift->shift, (unsigned)res);

    return res;
}

static void _ztimer_convert_shift_up_set(ztimer_clock_t *ztimer, uint32_t val)
{
    ztimer_convert_shift_t *ztimer_convert_shift =
        (ztimer_convert_shift_t *)ztimer;

    ztimer_set(ztimer_convert_shift->super.lower,
               &ztimer_convert_shift->super.lower_entry, _convert_shift_up_set(
                   ztimer_convert_shift,
                   val));
}

static uint32_t _ztimer_convert_shift_up_now(ztimer_clock_t *ztimer)
{
    ztimer_convert_shift_t *ztimer_convert_shift =
        (ztimer_convert_shift_t *)ztimer;

    return _convert_shift_up_now(ztimer_convert_shift,
                                 ztimer_now(ztimer_convert_shift->super.lower));
}

static const ztimer_ops_t _ztimer_convert_shift_ops_up = {
    .set = _ztimer_convert_shift_up_set,
    .now = _ztimer_convert_shift_up_now,
    .cancel = ztimer_convert_cancel,
};

void ztimer_convert_shift_up_init(ztimer_convert_shift_t *clock,
                                  ztimer_clock_t *lower, unsigned shift)
{
    uint32_t max_value = UINT32_MAX;

    DEBUG(
        "ztimer_convert_shift_init() shift=%i lower_maxval=%" PRIu32 "\n",
        shift, max_value);

    ztimer_convert_init(&clock->super, lower, max_value);
    clock->super.super.ops = &_ztimer_convert_shift_ops_up;
    clock->shift = shift;
}
