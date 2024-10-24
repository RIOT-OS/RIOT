/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           GPIO CPU definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_GPIO_H
#define PERIPH_CPU_GPIO_H

#include <stdint.h>
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */
#endif

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#if defined(CPU_FAM_STM32MP1)
#define GPIO_PIN(x, y)      ((GPIOA_BASE + (x << 12)) | y)
#else
#define GPIO_PIN(x, y)      ((GPIOA_BASE + (x << 10)) | y)
#endif

/**
 * @brief   Available GPIO ports
 */
enum {
#ifdef GPIOA
    PORT_A = 0,             /**< port A */
#endif
#ifdef GPIOB
    PORT_B = 1,             /**< port B */
#endif
#ifdef GPIOC
    PORT_C = 2,             /**< port C */
#endif
#ifdef GPIOD
    PORT_D = 3,             /**< port D */
#endif
#ifdef GPIOE
    PORT_E = 4,             /**< port E */
#endif
#ifdef GPIOF
    PORT_F = 5,             /**< port F */
#endif
#ifdef GPIOG
    PORT_G = 6,             /**< port G */
#endif
#ifdef GPIOH
    PORT_H = 7,             /**< port H */
#endif
#ifdef GPIOI
    PORT_I = 8,             /**< port I */
#endif
#ifdef GPIOJ
    PORT_J = 9,             /**< port J */
#endif
#ifdef GPIOK
    PORT_K = 10,            /**< port K */
#endif
};

/**
 * @brief   Available MUX values for configuring a pin's alternate function
 */
typedef enum {
#ifdef CPU_FAM_STM32F1
    GPIO_AF_OUT_PP = 0xb,   /**< alternate function output - push-pull */
    GPIO_AF_OUT_OD = 0xf,   /**< alternate function output - open-drain */
#else
    GPIO_AF0 = 0,           /**< use alternate function 0 */
    GPIO_AF1,               /**< use alternate function 1 */
    GPIO_AF2,               /**< use alternate function 2 */
    GPIO_AF3,               /**< use alternate function 3 */
    GPIO_AF4,               /**< use alternate function 4 */
    GPIO_AF5,               /**< use alternate function 5 */
    GPIO_AF6,               /**< use alternate function 6 */
    GPIO_AF7,               /**< use alternate function 7 */
#ifndef CPU_FAM_STM32F0
    GPIO_AF8,               /**< use alternate function 8 */
    GPIO_AF9,               /**< use alternate function 9 */
    GPIO_AF10,              /**< use alternate function 10 */
    GPIO_AF11,              /**< use alternate function 11 */
    GPIO_AF12,              /**< use alternate function 12 */
    GPIO_AF13,              /**< use alternate function 13 */
    GPIO_AF14,              /**< use alternate function 14 */
    GPIO_AF15,              /**< use alternate function 15 */
#endif
#endif
    GPIO_AF_UNDEF           /** an UNDEF value definition, e.g. for register
                                based spi */
} gpio_af_t;

#ifdef CPU_FAM_STM32F1
#ifndef DOXYGEN
/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 4 bit to determine the pin functions:
 * - bit 4: ODR value
 * - bit 2+3: in/out
 * - bit 1: PU enable
 * - bit 2: OD enable
 */
#define GPIO_MODE(mode, cnf, odr)       (mode | (cnf << 2) | (odr << 4))

/**
 * @brief   Override GPIO mode options
 *
 * We use 4 bit to encode CNF and MODE.
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 1, 0),    /**< input w/o pull R */
    GPIO_IN_PD = GPIO_MODE(0, 2, 0),    /**< input with pull-down */
    GPIO_IN_PU = GPIO_MODE(0, 2, 1),    /**< input with pull-up */
    GPIO_OUT   = GPIO_MODE(3, 0, 0),    /**< push-pull output */
    GPIO_OD    = GPIO_MODE(3, 1, 0),    /**< open-drain w/o pull R */
    GPIO_OD_PU = (0xff)                 /**< not supported by HW */
} gpio_mode_t;
/** @} */
#endif /* ndef DOXYGEN */

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
#else /* CPU_FAM_STM32F1 */
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
#endif /* ndef CPU_FAM_STM32F1 */

#ifndef DOXYGEN
/**
 * @brief   Override flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Configure the alternate function for the given pin
 *
 * @param[in] pin       pin to configure
 * @param[in] af        alternate function to use
 */
void gpio_init_af(gpio_t pin, gpio_af_t af);

/**
 * @brief   Configure the given pin to be used as ADC input
 *
 * @param[in] pin       pin to configure
 */
void gpio_init_analog(gpio_t pin);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CPU_GPIO_H */
