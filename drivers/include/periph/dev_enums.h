/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph
 *
 * @{
 * @file
 * @brief       Device enumerations for backward compatibility with existing
 *              peripheral driver implementations
 *
 * @todo        Remove this file once all peripheral drivers are ported to the
 *              more efficient implementations style
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_COMPAT_H_
#define PERIPH_COMPAT_H_

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Legacy definition of timers
 */
enum {
#if TIMER_0_EN
    TIMER_0,                /**< 1st timer */
#endif
#if TIMER_1_EN
    TIMER_1,                /**< 2nd timer */
#endif
#if TIMER_2_EN
    TIMER_2,                /**< 3rd timer */
#endif
#if TIMER_3_EN
    TIMER_3,                /**< 4th timer */
#endif
    TIMER_UNDEFINED         /**< deprecated legacy undefined values */
};

/**
 * @brief   Legacy definition of GPIO pins.
 */
enum {
#if GPIO_0_EN
    GPIO_0,                 /**< GPIO device 0 */
#endif
#if GPIO_1_EN
    GPIO_1,                 /**< GPIO device 1 */
#endif
#if GPIO_2_EN
    GPIO_2,                 /**< GPIO device 2 */
#endif
#if GPIO_3_EN
    GPIO_3,                 /**< GPIO device 3 */
#endif
#if GPIO_4_EN
    GPIO_4,                 /**< GPIO device 4 */
#endif
#if GPIO_5_EN
    GPIO_5,                 /**< GPIO device 5 */
#endif
#if GPIO_6_EN
    GPIO_6,                 /**< GPIO device 6 */
#endif
#if GPIO_7_EN
    GPIO_7,                 /**< GPIO device 7 */
#endif
#if GPIO_8_EN
    GPIO_8,                 /**< GPIO device 8 */
#endif
#if GPIO_9_EN
    GPIO_9,                 /**< GPIO device 9 */
#endif
#if GPIO_10_EN
    GPIO_10,                /**< GPIO device 10 */
#endif
#if GPIO_11_EN
    GPIO_11,                /**< GPIO device 11 */
#endif
#if GPIO_12_EN
    GPIO_12,                /**< GPIO device 12 */
#endif
#if GPIO_13_EN
    GPIO_13,                /**< GPIO device 13 */
#endif
#if GPIO_14_EN
    GPIO_14,                /**< GPIO device 14 */
#endif
#if GPIO_15_EN
    GPIO_15,                /**< GPIO device 15 */
#endif
#if GPIO_16_EN
    GPIO_16,                /**< GPIO device 16 */
#endif
#if GPIO_17_EN
    GPIO_17,                /**< GPIO device 17 */
#endif
#if GPIO_18_EN
    GPIO_18,                /**< GPIO device 18 */
#endif
#if GPIO_19_EN
    GPIO_19,                /**< GPIO device 19 */
#endif
#if GPIO_20_EN
    GPIO_20,                /**< GPIO device 20 */
#endif
#if GPIO_21_EN
    GPIO_21,                /**< GPIO device 21 */
#endif
#if GPIO_22_EN
    GPIO_22,                /**< GPIO device 22 */
#endif
#if GPIO_23_EN
    GPIO_23,                /**< GPIO device 23 */
#endif
#if GPIO_24_EN
    GPIO_24,                /**< GPIO device 24 */
#endif
#if GPIO_25_EN
    GPIO_25,                /**< GPIO device 25 */
#endif
#if GPIO_26_EN
    GPIO_26,                /**< GPIO device 26 */
#endif
#if GPIO_27_EN
    GPIO_27,                /**< GPIO device 27 */
#endif
#if GPIO_28_EN
    GPIO_28,                /**< GPIO device 28 */
#endif
#if GPIO_29_EN
    GPIO_29,                /**< GPIO device 29 */
#endif
#if GPIO_30_EN
    GPIO_30,                /**< GPIO device 30 */
#endif
#if GPIO_31_EN
    GPIO_31,                /**< GPIO device 31 */
#endif
    GPIO_NUMOF
};

/**
 * @brief   Legacy definitions of I2C devices
 */
enum {
#if I2C_0_EN
    I2C_0,                  /**< I2C device 0 */
#endif
#if I2C_1_EN
    I2C_1,                  /**< I2C device 1 */
#endif
#if I2C_2_EN
    I2C_2,                  /**< I2C device 2 */
#endif
#if I2C_3_EN
    I2C_3,                  /**< I2C device 3 */
#endif
    I2C_UNDEFINED           /**< Deprecated symbol, use I2C_UNDEF instead */
};

/**
 * @brief   Legacy definition of UART devices
 */
enum {
#if UART_0_EN
    UART_0 = 0,             /**< UART 0 */
#endif
#if UART_1_EN
    UART_1,                 /**< UART 1 */
#endif
#if UART_2_EN
    UART_2,                 /**< UART 2 */
#endif
#if UART_3_EN
    UART_3,                 /**< UART 3 */
#endif
#if UART_4_EN
    UART_4,                 /**< UART 4 */
#endif
#if UART_5_EN
    UART_5,                 /**< UART 5 */
#endif
#if UART_6_EN
    UART_6,                 /**< UART 6 */
#endif
    UART_UNDEFINED          /**< Deprecated symbol, use UART_UNDEF instead */
};

/**
 * @brief   Legacy definitions of PWM devices
 */
enum {
#if PWM_0_EN
    PWM_0,              /*< 1st PWM device */
#endif
#if PWM_1_EN
    PWM_1,              /*< 2nd PWM device */
#endif
#if PWM_2_EN
    PWM_2,              /*< 3rd PWM device */
#endif
#if PWM_3_EN
    PWM_3,              /*< 4th PWM device */
#endif
    PWM_UNDEFINED       /**< Deprecated symbol, use PWM_UNDEF instead */
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_COMPAT_H_ */
/** @} */
