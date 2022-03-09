/*
 * Copyright (C) 2019 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32v
 * @ingroup     drivers_periph_wdt
 *
 * @brief
 *
 * @{
 *
 * @file        wdt.c
 * @brief       Independent Watchdog timer for gd32v platforms
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include <stdlib.h>
#include <assert.h>
#include "macros/units.h"

#include "bitarithm.h"
#include "cpu.h"
#include "timex.h"

#include "periph_cpu.h"
#include "periph/wdt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define CLOCK_LSI                   (KHZ(40))

#define MAX_RELOAD              (4096U)
#define MAX_PRESCALER           (6U)
#define FWDGT_STEP_MS           ((4U * US_PER_MS * MAX_RELOAD) / CLOCK_LSI)

#define FWDGT_CTL_KEY_RELOAD    ((uint16_t)0xAAAA)
#define FWDGT_CTL_KEY_ENABLE    ((uint16_t)0xCCCC)

#define FWDGT_UNLOCK            ((uint16_t)0x5555)
#define FWDGT_LOCK              ((uint16_t)0x0000)

/* wdt_time (us) = LSI(us) x 4 x 2^PRE x RELOAD */
static inline __attribute__((used)) uint32_t _wdt_time(uint8_t pre, uint16_t rel)
{
    return (uint32_t)(((uint64_t)US_PER_SEC * 4 * (1 << pre) * rel) /
                      CLOCK_LSI);
}

static inline void _fwdt_unlock(void)
{
    FWDGT->CTL = FWDGT_UNLOCK;
}

static inline void _fwdt_lock(void)
{
    FWDGT->CTL = FWDGT_LOCK;
}

static void _set_prescaler(uint8_t prescaler)
{
    assert(prescaler <= MAX_PRESCALER);

    _fwdt_unlock();
    FWDGT->PSC = prescaler;
    _fwdt_lock();
}

static void _set_reload(uint16_t reload)
{
    assert(reload <= FWDGT_RLD_RLD_Msk);

    _fwdt_unlock();
    FWDGT->RLD = reload;
    _fwdt_lock();
}

static uint8_t _find_prescaler(uint32_t rst_time)
{
    /* Divide by the range to get power of 2 of the prescaler */
    uint8_t pre = bitarithm_msb(rst_time / FWDGT_STEP_MS) + 1;

    DEBUG("[wdt]: prescaler value %d\n", pre);
    return pre;
}

static uint16_t _find_reload_value(uint8_t pre, uint32_t rst_time)
{
    /* Calculate best reload value = rst_time / LSI(ms) x 4 x 2^PRE */
    uint16_t rel = (uint16_t)((rst_time * CLOCK_LSI) / \
                              ((uint32_t)(US_PER_MS * 4 * (1 << pre))));

    DEBUG("[wdt]: reload value %d\n", rel);
    return rel;
}

void wdt_start(void)
{
    FWDGT->CTL = FWDGT_CTL_KEY_ENABLE;
}

void wdt_kick(void)
{
    FWDGT->CTL = FWDGT_CTL_KEY_RELOAD;
}

void wdt_setup_reboot(uint32_t min_time, uint32_t max_time)
{
    (void)min_time;
    /* Windowed wdt not supported */
    assert(min_time == 0);

    /* Check reset time limit */
    assert((max_time > NWDT_TIME_LOWER_LIMIT) ||
           (max_time < NWDT_TIME_UPPER_LIMIT));

    RCU->RSTSCK |= RCU_RSTSCK_IRC40KEN_Msk;
    while (!(RCU->RSTSCK & RCU_RSTSCK_IRC40KSTB_Msk)) {}

    uint8_t pre = _find_prescaler(max_time);
    uint16_t rel = _find_reload_value(pre, max_time);

    /* Set watchdog prescaler and reload value */
    _set_prescaler(pre);
    _set_reload(rel);

    DEBUG("[wdt]: reset time %lu [us]\n", _wdt_time(pre, rel));

    /* Refresh wdt counter */
    wdt_kick();
}
