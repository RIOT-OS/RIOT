/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @ingroup     drivers_periph_wdt
 * @{
 *
 * @file
 * @brief       Implementation of the watchdog peripheral interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <inttypes.h>
#include <assert.h>

#include "cpu.h"
#include "timex.h"

#include "periph/wdt.h"

#include "vendor/aon.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void wdt_start(void)
{
    DEBUG("[wdt] start watchdog\n");

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCFG) |= AON_WDOGCFG_ENCOREAWAKE;
}

void wdt_stop(void)
{
    DEBUG("[wdt] stop watchdog\n");

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCFG) &= ~(AON_WDOGCFG_ENCOREAWAKE);
}

void wdt_kick(void)
{
    DEBUG("[wdt] reload the watchdog\n");

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGFEED) = AON_WDOGFEED_VALUE;
}

static inline uint8_t _scale(uint32_t count)
{
    uint8_t scale = 0;
    while (count > (UINT16_MAX - 1)) {
        count >>= 1;
        scale++;
    }

    return scale;
}

static inline uint8_t _setup(uint32_t min_time, uint32_t max_time)
{
    (void)min_time;

    /* Windowed wdt not supported */
    assert(min_time == 0);

    /* Check reset time limit */
    assert((max_time > NWDT_TIME_LOWER_LIMIT) || \
           (max_time < NWDT_TIME_UPPER_LIMIT));

    uint32_t count = ((uint32_t)max_time * RTC_FREQ) / MS_PER_SEC;
    uint8_t scale = _scale(count);

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCMP) = count;

    return scale;
}

void wdt_setup_reboot(uint32_t min_time, uint32_t max_time)
{
    uint8_t scale = _setup(min_time, max_time);

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCFG) = AON_WDOGCFG_RSTEN | AON_WDOGCFG_ZEROCMP | scale;

    DEBUG("[wdt] watchdog setup complete\n");
}
