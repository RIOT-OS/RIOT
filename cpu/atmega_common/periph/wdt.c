/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
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

#include "cpu.h"
#include "periph/pm.h"
#include "periph/wdt.h"

#include <avr/interrupt.h>
#include "avr/wdt.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

uint8_t wdt_prescaler = WDTO_15MS;

void wdt_start(void)
{
    DEBUG("[wdt] start the watchdog\n");
    wdt_enable(wdt_prescaler);
}

void wdt_stop(void)
{
    DEBUG("[wdt] stop the watchdog\n");
    wdt_disable();
}

void wdt_kick(void)
{
    DEBUG("[wdt] reload the watchdog\n");
    wdt_reset();
    wdt_enable(wdt_prescaler);
}

void wdt_setup_reboot(uint32_t min_time, uint32_t max_time)
{
    (void)min_time;

    /* Windowed wdt not supported */
    assert(min_time == 0);

    /* Check reset time limit */
    assert((max_time > NWDT_TIME_LOWER_LIMIT) || \
           (max_time < NWDT_TIME_UPPER_LIMIT));

    /* disable watchdog */
    wdt_disable();

    if (max_time >= 8000) {
        wdt_prescaler = WDTO_8S;
    }
    else if (max_time >= 4000) {
        wdt_prescaler = WDTO_4S;
    }
    else if (max_time >= 2000) {
        wdt_prescaler = WDTO_2S;
    }
    else if (max_time >= 1000) {
        wdt_prescaler = WDTO_1S;
    }
    else if (max_time >= 500) {
        wdt_prescaler = WDTO_500MS;
    }
    else if (max_time >= 250) {
        wdt_prescaler = WDTO_250MS;
    }
    else if (max_time >= 125) {
        wdt_prescaler = WDTO_120MS;
    }
    else if (max_time >= 64) {
        wdt_prescaler = WDTO_60MS;
    }
    else if (max_time >= 32) {
        wdt_prescaler = WDTO_30MS;
    }
    else {
        wdt_prescaler = WDTO_15MS;
    }

    DEBUG("[wdt] watchdog setup complete\n");
}
