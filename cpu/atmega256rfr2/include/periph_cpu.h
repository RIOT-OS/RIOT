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
 * @author          Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

#include "periph_cpu_common.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Available ports on the ATmega256rfr family
 */
enum {
    // PORT_A = 0,       /**< port A */ /* not connected to pins for megaRF*/
    PORT_B = 1,       /**< port B */
    // PORT_C = 2,       /**< port C */ /* not connected to pins for megaRF*/
    PORT_D = 3,       /**< port D */
    PORT_E = 4,       /**< port E */
    PORT_F = 5,       /**< port F */
    PORT_G = 6,       /**< port G */
};

/**
 * Struct definition for I2C
 */
typedef unsigned int gpio_t;

typedef struct{
	gpio_t scl_pin;
	gpio_t sda_pin;
	uint8_t *mask;
	mutex_t *used;
} i2c_conf_t;


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
