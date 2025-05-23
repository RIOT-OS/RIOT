/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_common_nrf51
 * @{
 *
 * @file
 * @brief       Common clock configuration for some nrf51 based boards
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "periph_cpu.h"

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
#define CLOCK_HFCLK         (16U)           /* set to  0: internal RC oscillator
                                                      16: 16MHz crystal
                                                      32: 32MHz crystal */
/* LFCLK Source clock selection:*/
/* - CLOCK_LFCLKSRC_SRC_RC: internal RC oscillator
 * - CLOCK_LFCLKSRC_SRC_Xtal: 32.768 kHz crystal
 * - CLOCK_LFCLKSRC_SRC_Synth: derived from HFCLK */
#define CLOCK_LFCLK         (CLOCK_LFCLKSRC_SRC_Xtal) /**< LFCLK Source */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
