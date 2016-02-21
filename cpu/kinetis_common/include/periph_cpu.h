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

#include "periph/dev_enums.h"
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
 * @brief   Definition of pin and port positions in the gpio_t type
 * @{
 */
#define GPIO_PORT_SHIFT     (8)
#define GPIO_PORT_MASK      (0xff << (GPIO_PORT_SHIFT))
#define GPIO_PIN_SHIFT      (0)
#define GPIO_PIN_MASK       (0xff << (GPIO_PIN_SHIFT))
/** @} */

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(port, pin) ((port << GPIO_PORT_SHIFT) | (pin << GPIO_PIN_SHIFT))

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @brief   Override values for pull register configuration
 * @{
 */
#define HAVE_GPIO_PP_T
typedef enum {
    GPIO_NOPULL = 0,                    /**< Normal push-pull (actively driven in both directions) */
    GPIO_OPEN_DRAIN = (PORT_PCR_ODE_MASK), /**< open drain without any pull resistors */
    GPIO_PULLUP = (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK), /**< enable internal pull-up resistor */
    GPIO_PULLDOWN = (PORT_PCR_PE_MASK),  /**< enable internal pull-down resistor */
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
typedef enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
    PORT_NUMOF
} port_t;

/**
 * @brief   Available MUX values for configuring a pin's alternate function
 */
typedef enum {
    PORT_AF0 = 0, /**< use alternate function 0 */
    PORT_AF1 = 1, /**< use alternate function 1 */
    PORT_AF2 = 2, /**< use alternate function 2 */
    PORT_AF3 = 3, /**< use alternate function 3 */
    PORT_AF4 = 4, /**< use alternate function 4 */
    PORT_AF5 = 5, /**< use alternate function 5 */
    PORT_AF6 = 6, /**< use alternate function 6 */
    PORT_AF7 = 7, /**< use alternate function 7 */
} port_af_t;

#ifndef PORT_AF_ANALOG
#define PORT_AF_ANALOG     PORT_AF0
#endif

/**
 * @brief Array of all PORT module base pointers
 */
extern PORT_Type * const kinetis_port_ptrs[];

static inline uint32_t gpio_port_num(gpio_t dev) {
    return (uint32_t)((dev & GPIO_PORT_MASK) >> GPIO_PORT_SHIFT);
}

static inline uint8_t gpio_pin_num(gpio_t dev) {
    return (uint8_t)((dev & GPIO_PIN_MASK) >> GPIO_PIN_SHIFT);
}

static inline PORT_Type *gpio_port(gpio_t dev) {
    return kinetis_port_ptrs[gpio_port_num(dev)];
}

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
