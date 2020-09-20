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

#ifndef PERIPH_DEV_ENUMS_H
#define PERIPH_DEV_ENUMS_H

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

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_DEV_ENUMS_H */
/** @} */
