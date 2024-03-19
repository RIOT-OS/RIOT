/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_tinyusb
 * @brief
 * @{
 *
 * @brief       nRF52 specific clock definitions as required by tinyUSB
 *
 * RIOT does not use the clock module from `pkg/nrfx/drivers`. Therefore
 * the part of the clock module interface needed by tinyUSB in
 * `tinyusb/src/portable/nordic/dcd_nrf5x.c` has to be defined explicitly.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef NRF52_NRF_CLOCK_H
#define NRF52_NRF_CLOCK_H

#include <stdbool.h>

#include "nrf.h"
#include_next "nrf_clock.h"

#if !DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#define NRF_CLOCK_HFCLK_HIGH_ACCURACY   (1UL)
#define NRF_CLOCK_EVENT_HFCLKSTARTED    offsetof(NRF_CLOCK_Type, EVENTS_HFCLKSTARTED)

typedef enum {
    NRF_CLOCK_TASK_HFCLKSTART,
    NRF_CLOCK_TASK_HFCLKSTOP,
} nrf_clock_task_t;

/**
 * @brief   Status HF clock activation/deactivation in `dcd_nrf52.c`
 *
 * The `clock_hfxo_request` and `clock_hfxo_release` functions are used in
 * RIOT to enable/disable the HF clock if necessary. Since `hfclk_enable`
 * in `tinyusb/src/portable/nordic/dcd_nrf5x.c` activates the RF clock only
 * if it is not already running, the status of the RF clock cannot be
 * determined via registers. It therefore needs its own static variable
 * that holds the current state of activation/deactivation by the function
 * `nrf_clock_task_trigger`.
 */
static bool _nrf_clock_hf_running = false;

/**
 * @brief   Check whether HF clock is running
 */
static inline bool nrf_clock_hf_is_running(NRF_CLOCK_Type const *reg,
                                           uint32_t clk_src)
{

    return _nrf_clock_hf_running;
}

/**
 * @brief   Clear a specific event
 *
 * This function is not required in RIOT, it is therefore defined as dummy
 * function.
 */
static inline bool nrf_clock_event_clear(NRF_CLOCK_Type *reg, uint32_t event)
{
    (void)reg;
    (void)event;
    return true;
}

/**
 * @brief   Function used in tinyUSB to start and stop the HF clock
 *
 * This function is mapped to `clock_hfxo_request`/`clock_hfxo_release` in RIOT.
 */
static inline void nrf_clock_task_trigger(NRF_CLOCK_Type *reg,
                                          nrf_clock_task_t task)
{
    switch (task) {
        case NRF_CLOCK_TASK_HFCLKSTART:
            clock_hfxo_request();
            _nrf_clock_hf_running = true;
            break;
        case NRF_CLOCK_TASK_HFCLKSTOP:
            clock_hfxo_release();
            _nrf_clock_hf_running = false;
            break;
        default:
            break;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* !DOXYGEN */
#endif /* NRF52_NRF_CLOCK_H */
/** @} */
