/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief       Clock initialization code
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "cpu.h"
#include "irq.h"
#include "assert.h"
#include "nrf_clock.h"
#include "periph_conf.h"

/* make HFCLK clock is configured */
#ifndef CLOCK_HFCLK
#error "Clock init: CLOCK_HFCLK is not defined by your board!"
#endif

/* Add compatibility wrapper defines for nRF families with Cortex-M33 core */
#ifdef NRF_CLOCK_S
#define NRF_CLOCK NRF_CLOCK_S
#endif

static unsigned _hfxo_requests = 0;

void clock_init_hf(void)
{
    /* for the nRF51 we can chose the XTAL frequency */
#ifdef CPU_FAM_NRF51
#if (CLOCK_HFCLK == 32)
    NRF_CLOCK->XTALFREQ = CLOCK_XTALFREQ_XTALFREQ_32MHz;
#elif (CLOCK_HFCLK == 16)
    NRF_CLOCK->XTALFREQ = CLOCK_XTALFREQ_XTALFREQ_16MHz;
#endif
#endif

    /* allow to always enable the HFXO as non-default option */
#if CLOCK_HFXO_ONBOOT
    clock_hfxo_request();
#endif
}

void clock_hfxo_request(void)
{
    unsigned state = irq_disable();
    ++_hfxo_requests;
    if (_hfxo_requests == 1) {
        NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
        NRF_CLOCK->TASKS_HFCLKSTART = 1;
        while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}
    }
    irq_restore(state);
}

void clock_hfxo_release(void)
{
    /* if this function is called while the counter is zero the state in a
     * driver requesting the HFXO is broken! */
    assert(_hfxo_requests);

    unsigned state = irq_disable();
    --_hfxo_requests;
    if (_hfxo_requests == 0) {
        NRF_CLOCK->TASKS_HFCLKSTOP = 1;
    }
    irq_restore(state);
}

/**
 * True if the low frequency clock (LFCLK) has been started by RIOT.
 * We don't rely on NRF_CLOCK->LFCLKSTAT since that register appears to be latched
 * for a short amount of time after a soft reset on at least nRF52832 and nRF52840.
 * @see https://devzone.nordicsemi.com/f/nordic-q-a/35792/nrf_clock--lfclkstat-register-contents-are-not-properly-evaluated-after-a-system-reset-if-rtc-compare-event-1-or-2-are-used/138995
 */
static bool clock_lf_running = false;

void clock_start_lf(void)
{
    /* abort if LF clock is already running */
    if (clock_lf_running) {
        return;
    }

    /* Select LFCLK source */
    NRF_CLOCK->LFCLKSRC = CLOCK_LFCLK;

    /* enable LF clock */
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {}
    clock_lf_running = true;

    /* calibrate the RC LF clock if applicable */
#if (CLOCK_HFCLK && (CLOCK_LFCLK == 0))
    NRF_CLOCK->EVENTS_DONE = 0;
    NRF_CLOCK->TASKS_CAL = 1;
    while (NRF_CLOCK->EVENTS_DONE == 0) {}
#endif
}

void clock_stop_lf(void)
{
    NRF_CLOCK->TASKS_LFCLKSTOP = 1;
    while (NRF_CLOCK->LFCLKSTAT & CLOCK_LFCLKSTAT_STATE_Msk) {}
    clock_lf_running = false;
}
