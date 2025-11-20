/*
 * SPDX-FileCopyrightText: 2017 Thomas Perrot <thomas.perrot@tupi.fr>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
 * @brief   Available ports on the ATmega32u4 family
 */
enum {
    PORT_B = 1,       /**< port B */
    PORT_C = 2,       /**< port C */
    PORT_D = 3,       /**< port D */
    PORT_E = 4,       /**< port E */
    PORT_F = 5        /**< port F */
};

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
 * @brief   Get the interrupt vector number of the given GPIO pin
 */
static inline uint8_t atmega_pin2exti(uint8_t port_num, uint8_t pin_num)
{
    (void)port_num;
    return pin_num;
}

/**
 * @brief   Check if the given pin can be used as external interrupt
 */
static inline bool atmega_has_pin_exti(uint8_t port_num, uint8_t pin_num)
{
    return ((pin_num < 4) && (port_num == PORT_D));
}

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

#include "periph_conf.h"
#include "default_timer_config.h"

/** @} */
