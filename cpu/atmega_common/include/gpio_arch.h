/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 Freie Universität Berlin
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega_common
 * @{
 *
 * @file
 * @brief           CPU specific definitions for GPIOs
 *
 * @author          René Herthel <rene-herthel@outlook.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef GPIO_ARCH_H
#define GPIO_ARCH_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

/**
 * @brief   Override register address type for GPIO ports of the MCU
 *
 * The size of this type must match the size of a pointer to distinguish
 * between MCU GPIO register addresses and pointers on GPIO devices.
 */
#define HAVE_GPIO_REG_T
typedef uint16_t gpio_reg_t;

/**
 * @brief   Override the GPIO flanks
 *
 * This device has an additional mode in which the interrupt is triggered
 * when the pin is low.
 *
 * Enumeration order is important, do not modify.
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_LOW,          /**< emit interrupt when pin low */
    GPIO_BOTH,         /**< emit interrupt on both flanks */
    GPIO_FALLING,      /**< emit interrupt on falling flank */
    GPIO_RISING,       /**< emit interrupt on rising flank */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* GPIO_ARCH_H */
/** @} */
