/*
 * Copyright (C) 2016 Technische Universität Braunschweig, IBR
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega1284p
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Robert Hartung <hartung@ibr.cs.tu-bs.de>
 */

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "periph_cpu_common.h"

/**
 * @brief   Available ports on the ATmega1281 family
 */
enum {
    PORT_A = 0,       /**< port A */
    PORT_B = 1,       /**< port B */
    PORT_C = 2,       /**< port C */
    PORT_D = 3,       /**< port D */
    PORT_E = 4,       /**< port E */
    PORT_F = 5,       /**< port F */
    PORT_G = 6,       /**< port G */
};

/**
 * @name   Defines for the I2C interface
 * @{
 */
#define I2C_PORT_REG            PORTD
#define I2C_PIN_MASK            (1 << PORTD1) | (1 << PORTD0)
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
