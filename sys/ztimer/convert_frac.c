/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Freie Universität Berlin
 *               2020 Inria
 *               2018 Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_ztimer_convert_frac
 *
 * @{
 *
 * @file
 * @brief       ztimer conversion using frac implementation
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdint.h>

#include "frac.h"
#include "assert.h"
#include "irq.h"
#include "ztimer/convert.h"
#include "ztimer/convert_frac.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Compute the scaling parameters for the given two frequencies
 *
 * @param[in]   self        pointer to instance to operate on
 * @param[in]   freq_self   desired frequency of this clock
 * @param[in]   freq_lower  frequency of the underlying clock
 */
static void ztimer_convert_frac_compute_scale(ztimer_convert_frac_t *self,
                                              uint32_t freq_self,
                                              uint32_t freq_lower);

static void ztimer_convert_frac_op_set(ztimer_clock_t *z, uint32_t val)
{
    ztimer_convert_frac_t *self = (ztimer_convert_frac_t *)z;
    uint32_t target_lower = frac_scale(&self->scale_set, val + self->round);

    DEBUG("ztimer_convert_frac_op_set(%" PRIu32 ")=%" PRIu32 "\n", val,
          target_lower);
    ztimer_set(self->super.lower, &self->super.lower_entry, target_lower);
}

static uint32_t ztimer_convert_frac_op_now(ztimer_clock_t *z)
{
    ztimer_convert_frac_t *self = (ztimer_convert_frac_t *)z;
    uint32_t lower_now = ztimer_now(self->super.lower);

    if (lower_now == 0) {
        return 0;
    }
    uint32_t scaled = frac_scale(&self->scale_now, lower_now);
    DEBUG("ztimer_convert_frac_op_now() %" PRIu32 "->%" PRIu32 "\n", lower_now,
          scaled);
    return scaled;
}

static const ztimer_ops_t ztimer_convert_frac_ops = {
    .set = ztimer_convert_frac_op_set,
    .now = ztimer_convert_frac_op_now,
    .cancel = ztimer_convert_cancel,
};

static void ztimer_convert_frac_compute_scale(ztimer_convert_frac_t *self,
                                              uint32_t freq_self,
                                              uint32_t freq_lower)
{
    assert(freq_self);
    assert(freq_lower);
    frac_init(&self->scale_now, freq_self, freq_lower);
    frac_init(&self->scale_set, freq_lower, freq_self);
}

void ztimer_convert_frac_init(ztimer_convert_frac_t *self,
                              ztimer_clock_t *lower, uint32_t freq_self,
                              uint32_t freq_lower)
{
    DEBUG("ztimer_convert_frac_init: %p->%p fs=%" PRIu32 " fl=%" PRIu32 "\n",
          (void *)self, (void *)lower, freq_self, freq_lower);

    *self = (ztimer_convert_frac_t) {
        .super.super = { .ops = &ztimer_convert_frac_ops, },
        .super.lower = lower,
        .super.lower_entry =
        { .callback = (void (*)(void *))ztimer_handler, .arg = &self->super, },
    };

    ztimer_convert_frac_compute_scale(self, freq_self, freq_lower);
    if (freq_self < freq_lower) {
        self->super.super.max_value = frac_scale(&self->scale_now, UINT32_MAX);
        ztimer_init_extend(&self->super.super);
    }
    else {
        DEBUG("ztimer_convert_frac_init: rounding up val:%" PRIu32 "\n",
              (uint32_t)(freq_self / freq_lower));
        self->round = freq_self / freq_lower;
        self->super.super.max_value = UINT32_MAX;
    }
}
