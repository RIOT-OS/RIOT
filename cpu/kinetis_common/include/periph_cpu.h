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

#include <stdint.h>

#include "cpu.h"

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

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      (((x + 1) << 12) | (x << 6) | y)

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @brief   Define a condensed set of PORT PCR values
 *
 * To combine values just aggregate them using a logical OR.
 */
enum {
    GPIO_AF_ANALOG = PORT_PCR_MUX(0),       /**< use pin as analog input */
    GPIO_AF_GPIO   = PORT_PCR_MUX(1),       /**< use pin as GPIO */
    GPIO_AF_2      = PORT_PCR_MUX(2),       /**< use alternate function 2 */
    GPIO_AF_3      = PORT_PCR_MUX(3),       /**< use alternate function 3 */
    GPIO_AF_4      = PORT_PCR_MUX(4),       /**< use alternate function 4 */
    GPIO_AF_5      = PORT_PCR_MUX(5),       /**< use alternate function 5 */
    GPIO_AF_6      = PORT_PCR_MUX(6),       /**< use alternate function 6 */
    GPIO_AF_7      = PORT_PCR_MUX(7),       /**< use alternate function 7 */
    GPIO_PCR_OD    = (PORT_PCR_ODE_MASK),   /**< open-drain mode */
    GPIO_PCR_PD    = (PORT_PCR_PE_MASK),    /**< enable pull-down */
    GPIO_PCR_PU    = (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK)  /**< enable PU */
};

/**
 * @brief   Override values for pull register configuration
 * @{
 */
#define HAVE_GPIO_PP_T
typedef enum {
    GPIO_NOPULL   = 0x0,            /**< do not use internal pull resistors */
    GPIO_PULLUP   = GPIO_PCR_PU,    /**< enable internal pull-up resistor */
    GPIO_PULLDOWN = GPIO_PCR_PD     /**< enable internal pull-down resistor */
} gpio_pp_t;
/** @} */

/**
 * @brief   Override flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING  = PORT_PCR_IRQC(0x9),  /**< emit interrupt on rising flank */
    GPIO_FALLING = PORT_PCR_IRQC(0xa),  /**< emit interrupt on falling flank */
    GPIO_BOTH    = PORT_PCR_IRQC(0xb),  /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */

/**
 * @brief   Available ports on the Kinetis family
 *
 * @todo    This is not equal for all members of the Kinetis family, right?
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
    GPIO_PORTS_NUMOF        /**< overall number of available ports */
};

/**
 * @brief   CPU internal function for initializing PORTs
 *
 * @param[in] pin       pin to initialize
 * @param[in] pcr       value for the PORT's PCR register
 */
void gpio_init_port(gpio_t pin, uint32_t pcr);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
