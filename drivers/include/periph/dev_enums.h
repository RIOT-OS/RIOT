/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 *
 * @{
 * @file
 * @brief       Device enumerations for backward compatibility with existing
 *              peripheral driver implementations
 *
 * TODO: Remove this file once all peripheral drivers are ported to the more
 *       efficient implementations style
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

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_COMPAT_H_ */
/** @} */
