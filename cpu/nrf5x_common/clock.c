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

/* make sure both clocks are configured */
#ifndef CLOCK_HFCLK
#error "Clock init: CLOCK_HFCLK is not defined by your board!"
#endif
#ifndef CLOCK_LFCLK
#error "Clock init: CLOCK_LFCLK is not defined by your board!"
#endif

/* Add compatibility wrapper defines for nRF9160 */
#ifdef NRF_CLOCK_S
#define NRF_CLOCK NRF_CLOCK_S
#endif

#ifdef CLOCK_LFCLKSRC_SRC_LFRC
#define CLOCK_LFCLKSRC_SRC_RC CLOCK_LFCLKSRC_SRC_LFRC
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

void clock_start_lf(void)
{
    /* abort if LF clock is already running */
    if (NRF_CLOCK->LFCLKSTAT & CLOCK_LFCLKSTAT_STATE_Msk) {
        return;
    }

#if (CLOCK_LFCLK == 0)
    NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_RC);
#elif (CLOCK_LFCLK == 1)
    NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal);
#elif (CLOCK_LFCLK == 2)
    NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Synth);
#elif (CLOCK_LFCLK == 3)
    NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_LFXO);
#else
#error "LFCLK init: CLOCK_LFCLK has invalid value"
#endif
    /* enable LF clock */
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {}

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
}
