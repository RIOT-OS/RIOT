/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_kinetis_common
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint16_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffff)

#define GPIO_PORT_SHIFT     (8)
#define GPIO_PORT_MASK      (0xff << (GPIO_PORT_SHIFT))
#define GPIO_PIN_SHIFT      (0)
#define GPIO_PIN_MASK       (0xff << (GPIO_PIN_SHIFT))

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(port, pin)          ((port << GPIO_PORT_SHIFT) | pin)

/**
 * @brief   Override values for pull register configuration
 * @{
 */
#define HAVE_GPIO_PP_T
typedef enum {
    GPIO_NOPULL = 4,        /**< do not use internal pull resistors */
    GPIO_PULLUP = 9,        /**< enable internal pull-up resistor */
    GPIO_PULLDOWN = 8       /**< enable internal pull-down resistor */
} gpio_pp_t;
/** @} */

/**
 * @brief   Override flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_LOGIC_ZERO = 0x8,  /**< interrupt on logic zero */
    GPIO_RISING     = 0x9,  /**< emit interrupt on rising flank */
    GPIO_FALLING    = 0xa,  /**< emit interrupt on falling flank */
    GPIO_BOTH       = 0xb,  /**< emit interrupt on both flanks */
    GPIO_LOGIC_ONE  = 0xc,  /**< interrupt on logic one */
} gpio_flank_t;
/** @} */

/**
 * @brief   Available ports on the Kinetis family
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
    PORT_NUMOF
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
