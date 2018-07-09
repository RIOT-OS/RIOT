/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 Freie Universität Berlin
 *               2018 Matthew Blue
 *               2018 Alan Antoine
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega644p
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          René Herthel <rene-herthel@outlook.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author          Alan Antoine <alan.antoine@free.fr>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)          ((x << 4) | y)

/**
 * @brief   Available ports on the ATmega644p family
 */
enum {
    PORT_A = 0,       /**< port A */
    PORT_B = 1,       /**< port B */
    PORT_C = 2,       /**< port C */
    PORT_D = 3        /**< port D */
};

/**
 * @brief   Available external interrupt pins on the ATmega644p family
 *
 * In order of their interrupt number
 */
#define CPU_ATMEGA_EXT_INTS    { GPIO_PIN(PORT_D, 2), \
                                 GPIO_PIN(PORT_D, 3), \
                                 GPIO_PIN(PORT_B, 2) }

/**
 * @name   Defines for the I2C interface
 * @{
 */
#define I2C_PORT_REG            PORTC
#define I2C_PIN_MASK            (1 << PORTC0) | (1 << PORTC1)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
