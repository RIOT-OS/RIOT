/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega2560
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          René Herthel <rene-herthel@outlook.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Available ports on the ATmega2560 family
 */
enum {
    PORT_A = 0,         /**< port A */
    PORT_B = 1,         /**< port B */
    PORT_C = 2,         /**< port C */
    PORT_D = 3,         /**< port D */
    PORT_E = 4,         /**< port E */
    PORT_F = 5,         /**< port F */
    PORT_G = 6,         /**< port G */
    PORT_H = 7,         /**< port H */
    PORT_J = 8,         /**< port J */
    PORT_K = 9,         /**< port K */
    PORT_L = 10,        /**< port L */
    GPIO_EXT_PORT = 11  /**< first GPIO extender port */
};

/**
 * @brief   Available ports on the ATmega2560 family as GPIO register definitions
 */
#define GPIO_CPU_PORTS \
    GPIO_CPU_PORT(0x22),  /* port A */ \
    GPIO_CPU_PORT(0x25),  /* port B */ \
    GPIO_CPU_PORT(0x28),  /* port C */ \
    GPIO_CPU_PORT(0x2b),  /* port D */ \
    GPIO_CPU_PORT(0x2e),  /* port E */ \
    GPIO_CPU_PORT(0x31),  /* port F */ \
    GPIO_CPU_PORT(0x34),  /* port G */ \
    GPIO_CPU_PORT(0x102), /* port H */ \
    GPIO_CPU_PORT(0x105), /* port J */ \
    GPIO_CPU_PORT(0x108), /* port K */ \
    GPIO_CPU_PORT(0x10b), /* port L */

/**
 * @brief   Available external interrupt pins on the ATmega2560 family
 *
 * In order of their interrupt number.
 */
#define CPU_ATMEGA_EXT_INTS    { GPIO_PIN(PORT_D, 0), \
                                 GPIO_PIN(PORT_D, 1), \
                                 GPIO_PIN(PORT_D, 2), \
                                 GPIO_PIN(PORT_D, 3), \
                                 GPIO_PIN(PORT_E, 4), \
                                 GPIO_PIN(PORT_E, 5), \
                                 GPIO_PIN(PORT_E, 6), \
                                 GPIO_PIN(PORT_E, 7) }

/**
 * @name   Defines for the I2C interface
 * @{
 */
#define I2C_PORT_REG            PORTD
#define I2C_PIN_MASK            (1 << PORTD0) | (1 << PORTD1)
/** @} */

/**
 * @name    EEPROM configuration
 * @{
 */
#define EEPROM_SIZE                (4096U)  /* 4kB */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
