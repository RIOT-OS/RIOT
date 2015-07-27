/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_ezr32wg
 * @{
 *
 * @file
 * @brief       CPU specific definitions for internal peripheral handling
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef CPU_PERIPH_H_
#define CPU_PERIPH_H_

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Override the timer undefined value
 */
#define TIMER_UNDEF         (0xffffffff)

/**
 * @brief   Override the timer type
 * @{
 */
#define HAVE_TIMER_T
typedef uint32_t tim_t;
/** @} */

/**
 * @brief   Define timer configuration values
 *
 * @note    The two timers must be adjacent to each other (e.g. TIMER0 and
 *          TIMER1, or TIMER2 and TIMER3, etc.).
 */
typedef struct {
    TIMER_TypeDef *prescaler;   /**< the lower numbered neighboring timer */
    TIMER_TypeDef *timer;       /**< the higher numbered timer */
    uint8_t pre_cmu;            /**< prescale timer bit in CMU register, the
                                 *   timer bit is deducted from this */
    uint8_t irqn;               /**< number of the higher timer IRQ channel */
} timer_conf_t;

/**
 * @brief   Define a custom type for GPIO pins
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Mandatory function for defining a GPIO pins
 * @{
 */
#define GPIO_PIN(x, y)      ((x << 4) | y)

/**
 * @brief   Available ports on the SAMD21
 */
enum {
    PA = 0,                 /**< port A */
    PB = 1,                 /**< port B */
    PC = 2,                 /**< port C */
    PD = 3,                 /**< port D */
    PE = 4,                 /**< port E */
    PF = 5                  /**< port F */
};

/**
 * @brief   Override direction values
 * @{
 */
#define HAVE_GPIO_DIR_T
typedef enum {
    GPIO_DIR_IN = 0,        /**< configure pin as input */
    GPIO_DIR_OUT = 4,       /**< configure pin as output */
} gpio_dir_t;
/** @} */

/**
 * @brief   Override pull register configuration values
 * @{
 */
#define HAVE_GPIO_PP_T
typedef enum {
    GPIO_NOPULL = 1,        /**< do not use internal pull resistors */
    GPIO_PULLUP = 6,        /**< enable internal pull-up resistor */
    GPIO_PULLDOWN = 2       /**< enable internal pull-down resistor */
} gpio_pp_t;
/** @} */

/**
 * @brief   Override active flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */

/**
 * @brief   UART device configuration
 */
typedef struct {
    USART_TypeDef *dev;     /**< USART device used */
    gpio_t rx_pin;          /**< Pin used for RX */
    gpio_t tx_pin;          /**< Pin used for TX */
    uint8_t loc;            /**< location of USART pins (AF) */
    uint8_t cmu;            /**< the device CMU channel */
    uint8_t irq;            /**< the devices base IRQ channel */
} uart_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* CPU_PERIPH_H_ */
/** @} */
