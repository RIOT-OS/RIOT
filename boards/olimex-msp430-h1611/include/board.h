/*
 * Copyright (C) 2013, 2014 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef BOARD_H
#define BOARD_H

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
 * @brief   Configure clock to ~1 MHz.
 *
 * HACK: Do not use exactly 1 MHz to force use of ztimer_convert_frac, as the
 * internal oscillator is not precise enough for time keeping and compensation
 * based on the actual measured CPU frequency is needed.
 */
#define CONFIG_ZTIMER_USEC_BASE_FREQ    (MHZ(1) + 1)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
