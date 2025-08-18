/*
 * SPDX-FileCopyrightText: 2013-2014 INRIA
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_olimex_msp430_h1611
 *
 * @{
 *
 * @file
 * @brief       Basic definitions for the Olimex MSP430-H1611 board
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define the CPU model for the <msp430.h>
 */
#ifndef __MSP430F1611__
#define __MSP430F1611__
#endif

/**
 * @name    ztimer configuration
 * @{
 */
#define CONFIG_ZTIMER_USEC_WIDTH        16      /**< running on a 16-bit timer */
/**
 * @brief   Configure clock to 1 MHz.
 */
#define CONFIG_ZTIMER_USEC_BASE_FREQ    MHZ(1)
/**
 * @brief   Force frequency conversion to be used
 *
 * Otherwise ztimer would assume that it can run the 1 MHz clock from the timer
 * configured at 1 MHz without conversion. But the actual frequency the timer
 * will run at will be detected at runtime and, due to inaccuracy of the
 * internal oscillator, will indeed require frequency conversion.
 */
#define CONFIG_ZTIMER_PERIPH_TIMER_FORCE_CONVERSION     1
/** @} */

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (40)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
