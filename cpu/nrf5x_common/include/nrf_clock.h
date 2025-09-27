/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief       nRF5x shared functions for configuration the system clocks
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The high frequency clock (HFCLK) uses the internal oscillator per
 *          default. Setting this define to 1 will enable the HFXO clock source
 *          on boot so it will always be active.
 */
#ifndef CLOCK_HFXO_ONBOOT
#define CLOCK_HFXO_ONBOOT       0
#endif

/**
 * @brief   Initialize the high frequency clock (HFCLK) as configured in the
 *          board's periph_conf.h
 */
void clock_init_hf(void);

/**
 * @brief   Request the external high frequency crystal (HFXO) as HF clock
 *          source. If this is the first request, the HFXO will be enabled.
 */
void clock_hfxo_request(void);

/**
 * @brief   Release the use of the HFXO. If this was the last active request,
 *          the HFXO will be disabled
 */
void clock_hfxo_release(void);

/**
 * @brief   Start the low frequency clock (LFCLK) as configured in the board's
 *          periph_conf.
 *
 * Calling this function while the LFCLK is already running will have no effect.
 */
void clock_start_lf(void);

/**
 * @brief   Stop the low frequency clock (LFCLK)
 *
 * @note    Be sure that no module is using the LFCLK before you shut it down!
 */
void clock_stop_lf(void);

#ifdef __cplusplus
}
#endif

/** @} */
