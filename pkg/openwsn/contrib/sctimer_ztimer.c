/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       Ztimer based adaptation of "sctimer" bsp module
 *
 * For details on the implementation check pkg/openwsn/doc.txt
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */
#include "sctimer.h"
#include "debugpins.h"

#include "ztimer.h"
#include "ztimer/convert.h"
#include "ztimer/convert_frac.h"
#include "ztimer/convert_shift.h"
#include "ztimer/config.h"

#include "periph_conf.h"

#define LOG_LEVEL LOG_NONE
#include "log.h"

/**
 * @brief   Maximum counter difference to not consider an ISR late, this
 *          should account for the largest timer interval OpenWSN
 *          scheduler might work with. When running only the stack this
 *          should not be more than SLOT_DURATION, but when using cjoin
 *          it is 65535ms
 */
#ifndef SCTIMER_LOOP_THRESHOLD
#define SCTIMER_LOOP_THRESHOLD       (2 * PORT_TICS_PER_MS * 65535)
#endif

#if CONFIG_ZTIMER_MSEC_BASE_FREQ > 32768U
static ztimer_convert_frac_t _ztimer_convert_frac_32768;
#define ZTIMER_32768_CONVERT_LOWER_FREQ    CONFIG_ZTIMER_MSEC_BASE_FREQ
#define ZTIMER_32768_CONVERT_LOWER         (ZTIMER_MSEC_BASE)
/* cppcheck-suppress preprocessorErrorDirective
 * (reason: cppcheck fails to see that CONFIG_ZTIMER_MSEC_BASE_FREQ
 * is set in ztimer/config.h to a non zero value */
#elif (CONFIG_ZTIMER_MSEC_BASE_FREQ < 32768U) && \
    ((32768U % CONFIG_ZTIMER_MSEC_BASE_FREQ) == 0)
static ztimer_convert_shift_t _ztimer_convert_shift_32768;
#define ZTIMER_32768_CONVERT_HIGHER_FREQ   CONFIG_ZTIMER_MSEC_BASE_FREQ
#define ZTIMER_32768_CONVERT_HIGHER        (ZTIMER_MSEC_BASE)
#elif (CONFIG_ZTIMER_MSEC_BASE_FREQ < 32768U) && \
    ((32768U % CONFIG_ZTIMER_MSEC_BASE_FREQ) != 0)
#error No suitable ZTIMER_MSEC_BASE config. Maybe add USEMODULE += ztimer_usec?
#endif

static sctimer_cbt _sctimer_cb;
static ztimer_t _ztimer_sctimer;
static ztimer_clock_t *ZTIMER_32768 = NULL;

static void sctimer_isr_internal(void *arg)
{
    (void)arg;

    if (_sctimer_cb != NULL) {
        debugpins_isr_set();
        _sctimer_cb();
        debugpins_isr_clr();
    }
}

void sctimer_init(void)
{
#if CONFIG_ZTIMER_MSEC_BASE_FREQ > 32768U
    ZTIMER_32768 = &_ztimer_convert_frac_32768.super.super;
/* cppcheck-suppress preprocessorErrorDirective
 * (reason: cppcheck fails to see that CONFIG_ZTIMER_MSEC_BASE_FREQ
 * is set in ztimer/config.h to a non zero value */
#elif (CONFIG_ZTIMER_MSEC_BASE_FREQ < 32768U) && \
    (32768U % CONFIG_ZTIMER_MSEC_BASE_FREQ == 0)
    ZTIMER_32768 = &_ztimer_convert_shift_32768.super.super;
#elif CONFIG_ZTIMER_MSEC_BASE_FREQ == 32768U
    ZTIMER_32768 = ZTIMER_MSEC_BASE;
#else
#error Invalid ZTIMER_MSEC_BASE_FREQ config. Maybe add USEMODULE += ztimer_usec?
#endif

#if defined(ZTIMER_32768_CONVERT_LOWER)
    LOG_DEBUG("[sctimer]: ZTIMER_32768 convert_frac from %lu to 32768\n",
              (long unsigned)ZTIMER_32768_CONVERT_LOWER_FREQ);
    ztimer_convert_frac_init(&_ztimer_convert_frac_32768,
                             ZTIMER_32768_CONVERT_LOWER,
                             SCTIMER_FREQUENCY,
                             ZTIMER_32768_CONVERT_LOWER_FREQ);
#elif defined(ZTIMER_32768_CONVERT_HIGHER)
    LOG_DEBUG("[sctimer]: ZTIMER_32768 convert_shift %lu to 32768\n",
              (long unsigned)ZTIMER_32768_CONVERT_HIGHER_FREQ);
    ztimer_convert_shift_up_init(&_ztimer_convert_shift_32768,
                                 ZTIMER_32768_CONVERT_HIGHER,
                                 __builtin_ctz(SCTIMER_FREQUENCY /
                                               CONFIG_ZTIMER_MSEC_BASE_FREQ));
#endif

    _ztimer_sctimer.callback = sctimer_isr_internal;
}

void sctimer_set_callback(sctimer_cbt cb)
{
    _sctimer_cb = cb;
}

void sctimer_setCompare(uint32_t val)
{
    unsigned state = irq_disable();

    uint32_t now = ztimer_now(ZTIMER_32768);

    /* if the next compare value (isr) to schedule is already later than
       the required value, but close enough to think we have been slow
       in scheduling it, trigger the ISR right away */
    if (now > val) {
        if (now - val < SCTIMER_LOOP_THRESHOLD) {
            ztimer_set(ZTIMER_32768, &_ztimer_sctimer, 0);
        }
        else {
            ztimer_set(ZTIMER_32768, &_ztimer_sctimer,
                       UINT32_MAX - now + val);
        }
    }
    else {
        ztimer_set(ZTIMER_32768, &_ztimer_sctimer, val - now);
    }

    irq_restore(state);

    LOG_DEBUG("[sctimer]: set cb to %" PRIu32 " at %" PRIu32 "\n",
              val, now);
}

uint32_t sctimer_readCounter(void)
{
    uint32_t now = ztimer_now(ZTIMER_32768);

    LOG_DEBUG("[sctimer]: now %" PRIu32 "\n", now);
    return now;
}

void sctimer_enable(void)
{
    /* not supported, sctimer does not control RTT or ztimer */
}

void sctimer_disable(void)
{
    /* not supported, sctimer does not control RTT or ztimer */
}
