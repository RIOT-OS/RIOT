/*
 * Copyright (C) Josua Arndt, Steffen Robertz 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega256rfr2
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author          Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"
#include "atmega_regs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Available ports on the ATmega256rfr family
 * @{
 */
enum {
    PORT_B  = 1,        /**< port B */
    PORT_D  = 3,        /**< port D */
    PORT_E  = 4,        /**< port E */
    PORT_F  = 5,        /**< port F */
    PORT_G  = 6,        /**< port G */
};
/** @} */

/**
 * @brief   Available external interrupt pins on the ATmega256rfr family
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
#define I2C_PIN_MASK            (1 << PORTD1) | (1 << PORTD0)
/** @} */

/**
 * @name    EEPROM configuration
 * @{
 */
#define EEPROM_SIZE             (8192U)  /* 8kB */
/** @} */

#ifdef __cplusplus
}
#endif

#include "periph_conf.h"
#include "default_timer_config.h"

#endif /* PERIPH_CPU_H */
/** @} */
