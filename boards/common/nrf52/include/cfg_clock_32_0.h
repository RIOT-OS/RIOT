/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_nrf52
 * @{
 *
 * @file
 * @brief       Common clock configuration for the nRF52 based boards
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 *
 * @note    The radio will not work with the internal RC oscillator!
 *
 * @{
 */
#define CLOCK_HFCLK         (32U)           /* set to  0: internal RC oscillator
                                             *        32: 32MHz crystal */
/* LFCLK Source clock selection:*/
/* - CLOCK_LFCLKSRC_SRC_RC: internal RC oscillator
 * - CLOCK_LFCLKSRC_SRC_Xtal: 32.768 kHz crystal
 * - CLOCK_LFCLKSRC_SRC_Synth: derived from HFCLK */
#define CLOCK_LFCLK         (CLOCK_LFCLKSRC_SRC_RC) /**< LFCLK Source */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
