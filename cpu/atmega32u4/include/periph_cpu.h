/*
 * Copyright (C) 2017 Thomas Perrot <thomas.perrot@tupi.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega32u4
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Thomas Perrot <thomas.perrot@tupi.fr>
 *
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Available ports on the ATmega32u4 family
 */
enum {
    PORT_B = 1,       /**< port B */
    PORT_C = 2,       /**< port C */
    PORT_D = 3,       /**< port D */
    PORT_E = 4,       /**< port E */
    PORT_F = 5,       /**< port F */
    GPIO_EXT_PORT = 6 /**< first GPIO extender port */
};

/**
 * @brief   Available ports on the ATmega32u4 family as GPIO register definitions
 */
#define GPIO_CPU_PORTS \
    GPIO_CPU_PORT(0   ), /* n/a    */ \
    GPIO_CPU_PORT(0x25), /* port B */ \
    GPIO_CPU_PORT(0x28), /* port C */ \
    GPIO_CPU_PORT(0x2b), /* port D */ \
    GPIO_CPU_PORT(0x2e), /* port E */ \
    GPIO_CPU_PORT(0x31), /* port F */

/**
 * @brief   Available external interrupt pins on the ATmega32u4 family
 *
 * In order of their interrupt number.
 */
#define CPU_ATMEGA_EXT_INTS    { GPIO_PIN(PORT_D, 0), \
                                 GPIO_PIN(PORT_D, 1), \
                                 GPIO_PIN(PORT_D, 2), \
                                 GPIO_PIN(PORT_D, 3) }

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
#define EEPROM_SIZE                (1024U)  /* 1kB */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
