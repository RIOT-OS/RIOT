/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atxmega
 * @brief       Common implementations and headers for ATxmega family based micro-controllers
 * @{
 *
 * @file
 * @brief       Basic definitions for the ATxmega common clock module
 *
 * When ever you want to do something hardware related, that is accessing MCUs registers directly,
 * just include this file. It will then make sure that the MCU specific headers are included.
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 */

#ifndef CPU_CLOCK_H
#define CPU_CLOCK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ATxmega system clock prescaler settings
 *
 * Some CPUs may not support the highest prescaler settings
 */
enum {
    CPU_ATXMEGA_CLK_SCALE_DIV1      = 0,
    CPU_ATXMEGA_CLK_SCALE_DIV2      = 1,
    CPU_ATXMEGA_CLK_SCALE_DIV4      = 3,
    CPU_ATXMEGA_CLK_SCALE_DIV8      = 5,
    CPU_ATXMEGA_CLK_SCALE_DIV16     = 7,
    CPU_ATXMEGA_CLK_SCALE_DIV32     = 9,
    CPU_ATXMEGA_CLK_SCALE_DIV64     = 11,
    CPU_ATXMEGA_CLK_SCALE_DIV128    = 13,
    CPU_ATXMEGA_CLK_SCALE_DIV256    = 15,
    CPU_ATXMEGA_CLK_SCALE_DIV512    = 17,
};

enum {
    CPU_ATXMEGA_BUS_SCALE_DIV1_1    = 0,
    CPU_ATXMEGA_BUS_SCALE_DIV1_2    = 1,
    CPU_ATXMEGA_BUS_SCALE_DIV4_1    = 2,
    CPU_ATXMEGA_BUS_SCALE_DIV2_2    = 3,
};

/**
 * @brief   Initializes system clock prescaler
 */
static inline void atxmega_set_prescaler(uint8_t clk_scale, uint8_t bus_scale)
{
    /* Disable CCP for Protected IO register and set new value
     * Set system clock prescalers to zero. PSCTRL contains A Prescaler
     * Value and one value for and B and C Prescaler
     */
    _PROTECTED_WRITE(CLK.PSCTRL, clk_scale | bus_scale);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_CLOCK_H */
