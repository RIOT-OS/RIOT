/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2018 RWTH Aachen, Josua Arndt <jarndt@ias.rwth-aachen.de>
 *               2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @brief       Common clock support for ATmega family based micro-controllers
 * @{
 *
 * @file
 * @brief       Basic definitions for the ATmega common clock
 *
 * When ever you want to do something hardware related, that is accessing MCUs registers directly,
 * just include this file. It will then make sure that the MCU specific headers are included.
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 */

#ifndef CPU_CLOCK_H
#define CPU_CLOCK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   ATmega system clock prescaler settings
 *
 * Some CPUs may not support the highest prescaler settings
 */
enum {
    CPU_ATMEGA_CLK_SCALE_DIV1   = 0,
    CPU_ATMEGA_CLK_SCALE_DIV2   = 1,
    CPU_ATMEGA_CLK_SCALE_DIV4   = 2,
    CPU_ATMEGA_CLK_SCALE_DIV8   = 3,
    CPU_ATMEGA_CLK_SCALE_DIV16  = 4,
    CPU_ATMEGA_CLK_SCALE_DIV32  = 5,
    CPU_ATMEGA_CLK_SCALE_DIV64  = 6,
    CPU_ATMEGA_CLK_SCALE_DIV128 = 7,
    CPU_ATMEGA_CLK_SCALE_DIV256 = 8,
    CPU_ATMEGA_CLK_SCALE_DIV512 = 9,
};

/**
 * @brief   Initializes system clock prescaler
 */
static inline void atmega_set_prescaler(uint8_t clk_scale)
{
    /* Enable clock change */
#ifdef CLKPR
    /* Must be assignment to set all other bits to zero, see datasheet */
    CLKPR = (1 << CLKPCE);

    /* Write clock within 4 cycles */
    CLKPR = clk_scale;
#else
    (void) clk_scale;
#endif
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_CLOCK_H */
