/*
 * SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_ztimer_convert
 * @{
 *
 * @file
 * @brief       ztimer frequency conversion module common code implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "ztimer/convert.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void ztimer_convert_cancel(ztimer_clock_t *clock)
{
    ztimer_convert_t *ztimer_convert = (ztimer_convert_t *)clock;

    ztimer_remove(ztimer_convert->lower, &ztimer_convert->lower_entry);
}

void ztimer_convert_start(ztimer_clock_t *clock)
{
    ztimer_convert_t *ztimer_convert = (ztimer_convert_t *)clock;

    ztimer_acquire(ztimer_convert->lower);
}

void ztimer_convert_stop(ztimer_clock_t *clock)
{
    ztimer_convert_t *ztimer_convert = (ztimer_convert_t *)clock;

    ztimer_release(ztimer_convert->lower);
}

void ztimer_convert_init(ztimer_convert_t *ztimer_convert,
                         ztimer_clock_t *lower,
                         uint32_t max_value)
{
    ztimer_convert_t tmp = {
        .lower = lower,
        .lower_entry = {
            .callback = (void (*)(void *)) ztimer_handler,
            .arg = ztimer_convert,
        },
        .super.max_value = max_value,
#  if MODULE_PM_LAYERED && !MODULE_ZTIMER_ONDEMAND
        .super.block_pm_mode = ZTIMER_CLOCK_NO_REQUIRED_PM_MODE,
#  endif
    };

    *ztimer_convert = tmp;

    DEBUG("ztimer_convert_init() max_value=%" PRIu32 "\n",
          ztimer_convert->super.max_value);
}
