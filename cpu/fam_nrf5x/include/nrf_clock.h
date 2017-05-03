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
 * @brief       nRF5x shared functions for configuration the system clocks
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NRF_CLOCK_H
#define NRF_CLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the high frequency clock (HFCLK) as configured in the
 *          board's periph_conf.h
 */
void clock_init_hf(void);

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

#endif /* NRF_CLOCK_H */
/** @} */
