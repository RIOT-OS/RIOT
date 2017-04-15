/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32f3
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 5 bit to encode the mode:
 * - bit 0+1: pin mode (input / output)
 * - bit 2+3: pull resistor configuration
 * - bit   4: output type (0: push-pull, 1: open-drain)
 */
#define GPIO_MODE(io, pr, ot)   ((io << 0) | (pr << 2) | (ot << 4))

#ifndef DOXYGEN
/**
 * @brief   Override GPIO mode options
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 0, 0),    /**< input w/o pull R */
    GPIO_IN_PD = GPIO_MODE(0, 2, 0),    /**< input with pull-down */
    GPIO_IN_PU = GPIO_MODE(0, 1, 0),    /**< input with pull-up */
    GPIO_OUT   = GPIO_MODE(1, 0, 0),    /**< push-pull output */
    GPIO_OD    = GPIO_MODE(1, 0, 1),    /**< open-drain w/o pull R */
    GPIO_OD_PU = GPIO_MODE(1, 1, 1)     /**< open-drain with pull-up */
} gpio_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Available ports on the STM32F3 family
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
};

/**
 * @brief   DAC line configuration support
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the line */
    uint8_t dac;            /**< The DAC to use, 0 or 1 */
    uint8_t chan;           /**< DAC device used for this line */
} dac_conf_t;

/**
 * @brief   Configure the alternate function for the given pin
 *
 * @note    This is meant for internal use in STM32F4 peripheral drivers only
 *
 * @param[in] pin       pin to configure
 * @param[in] af        alternate function to use
 */
void gpio_init_af(gpio_t pin, gpio_af_t af);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
