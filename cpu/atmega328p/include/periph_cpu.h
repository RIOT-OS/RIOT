/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega328p
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
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)          ((x << 4) | y)

/**
 * @brief   Available ports on the ATmega328p family
 */
enum {
    PORT_B = 1,       /**< port B */
    PORT_C = 2,       /**< port C */
    PORT_D = 3        /**< port D */
};

/**
 * @brief   Available external interrupt pins on the ATmega328p family
 *
 * In order of their interrupt number.
 */
#define CPU_ATMEGA_EXT_INTS    { GPIO_PIN(PORT_D, 2), \
                                 GPIO_PIN(PORT_D, 3) }

/**
 * @brief   Get the interrupt vector number of the given GPIO pin
 */
static inline uint8_t atmega_pin2exti(uint8_t port_num, uint8_t pin_num)
{
    (void)port_num;
    return pin_num - 2;
}

/**
 * @brief   Check if the given pin can be used as external interrupt
 */
static inline bool atmega_has_pin_exti(uint8_t port_num, uint8_t pin_num)
{
    if (port_num == PORT_D) {
        return ((pin_num == 2) || (pin_num == 3));
    }

    return false;
}

/**
 * @name   Defines for the I2C interface
 * @{
 */
#define I2C_PORT_REG            PORTC
#define I2C_PIN_MASK            (1 << PORTC4) | (1 << PORTC5)
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

#include "periph_conf.h"
#include "default_timer_config.h"

#endif /* PERIPH_CPU_H */
/** @} */
