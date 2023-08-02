/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
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

#include <assert.h>

#include "cpu.h"
#include "timex.h"
#include "periph/wdt.h"

#include "nrf_clock.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* By default, allow watchdog during sleep.
   Warning: pausing the watchdog during sleep will deactivate it when RIOT
   switches to idle thread where pm_set_lowest is called. */
#ifndef NRF_WDT_SLEEP_MODE
#define NRF_WDT_SLEEP_MODE (WDT_CONFIG_SLEEP_Run)
#endif

/* By default, allow watchdog during debug session. */
#ifndef NRF_WDT_HALT_MODE
#define NRF_WDT_HALT_MODE (WDT_CONFIG_HALT_Run)
#endif

/* Compatibility wrapper for nRF53/nRF9160 */
#ifdef NRF_WDT0_S
#define NRF_WDT NRF_WDT0_S
#elif defined(NRF_WDT_S)
#define NRF_WDT NRF_WDT_S
#endif

/* Wrapper around vendor files inconsistency */
#ifdef WDT_RUNSTATUS_RUNSTATUSWDT_Running
#define WDT_RUNSTATUS_RUNSTATUS_Running WDT_RUNSTATUS_RUNSTATUSWDT_Running
#endif

#ifdef MODULE_PERIPH_WDT_CB
static wdt_cb_t wdt_cb;
static void *wdt_arg;
#endif

void wdt_start(void)
{
    DEBUG("[wdt] start watchdog\n");

    NRF_WDT->TASKS_START = 1;
}

void wdt_stop(void)
{
    DEBUG("[wdt] stopping the watchdog is not supported\n");
    assert(0);
}

void wdt_kick(void)
{
    assert(NRF_WDT->RUNSTATUS == WDT_RUNSTATUS_RUNSTATUS_Running);

    DEBUG("[wdt] reload the watchdog\n");

    NRF_WDT->RR[0] = WDT_RR_RR_Reload;
}

void wdt_setup_reboot(uint32_t min_time, uint32_t max_time)
{
    (void)min_time;

    /* Windowed wdt not supported */
    assert(min_time == 0);

    /* Check reset time limit */
    assert((max_time > NWDT_TIME_LOWER_LIMIT) || \
           (max_time < NWDT_TIME_UPPER_LIMIT));

    /* configure watchdog behavior during sleep */
    NRF_WDT->CONFIG &= ~(WDT_CONFIG_SLEEP_Msk << WDT_CONFIG_SLEEP_Pos);
    NRF_WDT->CONFIG |= (NRF_WDT_SLEEP_MODE << WDT_CONFIG_SLEEP_Pos);

    /* configure watchdog behavior during debug */
    NRF_WDT->CONFIG &= ~(WDT_CONFIG_HALT_Msk << WDT_CONFIG_HALT_Pos);
    NRF_WDT->CONFIG |= (NRF_WDT_HALT_MODE << WDT_CONFIG_HALT_Pos);

    /* timeout (s) = (CRV + 1) / 32768 */
    uint32_t crv = ((max_time << 15) / 1000) - 1;
    DEBUG("[wdt] setting CRV to %"PRIu32"\n", crv);
    NRF_WDT->CRV = crv;

    DEBUG("[wdt] CRV configuration: %"PRIu32"\n", NRF_WDT->CRV);

    /* Enable reload requests */
    NRF_WDT->RREN = (WDT_RREN_RR0_Enabled << WDT_RREN_RR0_Pos);

    DEBUG("[wdt] watchdog setup complete\n");
}

#ifdef MODULE_PERIPH_WDT_CB
/* The reset can't be stopped when the callback is triggered: so the MCU will
   reset in any case after 2 cycles of 32.768kHz clock. This is very short so
   only basic and fast operations can be performed in the callback function. */
void wdt_setup_reboot_with_callback(uint32_t min_time, uint32_t max_time,
                                    wdt_cb_t cb, void* arg)
{
    wdt_cb = cb;
    wdt_arg = arg;

    /* Disable interrupt */
    NVIC_DisableIRQ(WDT_IRQn);

    if (cb) {
        /* enable interrupt */
        NVIC_EnableIRQ(WDT_IRQn);
        NRF_WDT->INTENSET = WDT_INTENSET_TIMEOUT_Enabled;
    }

    wdt_setup_reboot(min_time, max_time);
}

void isr_wdt(void)
{
    wdt_cb(wdt_arg);

    cortexm_isr_end();
}
#endif /* MODULE_PERIPH_WDT_CB */
