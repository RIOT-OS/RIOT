/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 Freie Universität Berlin
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_atmega8
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          René Herthel <rene-herthel@outlook.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Hugues Larrive <hugues.larrive@pm.me>
 */


#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power management configuration
 * @{
 */
#define PM_NUM_MODES            (5)
#define AVR8_PM_SLEEP_MODE_0    SLEEP_MODE_PWR_DOWN /**< Power Down */
#define AVR8_PM_SLEEP_MODE_1    SLEEP_MODE_PWR_SAVE /**< Power Save */
#define AVR8_PM_SLEEP_MODE_2    SLEEP_MODE_STANDBY  /**< Standby */
#define AVR8_PM_SLEEP_MODE_3    SLEEP_MODE_ADC      /**< Sleep ADC low noise */
/** @} */

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)          ((x << 4) | y)

/**
 * @brief   Available ports on the ATmega8 family
 */
enum {
    PORT_B = 1,       /**< port B */
    PORT_C = 2,       /**< port C */
    PORT_D = 3        /**< port D */
};

/**
 * @brief   Available external interrupt pins on the ATmega8 family
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
#define EEPROM_SIZE                (512U)  /* 512B */
/** @} */

#ifdef __cplusplus
}
#endif

#include "periph_conf.h"
#include "default_timer_config.h"

/** @} */
