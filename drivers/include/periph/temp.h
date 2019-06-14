/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_temp Internal temperature sensor driver
 * @ingroup     drivers_periph
 * @brief       Peripheral hardware internal temperature interface
 *
 * The internal temperature interface provides an easy way to get a rough
 * approximation of internal temperature. Accuracy may vary a lot between
 * MCUs families. Check your MCU datasheet to have more informations about range
 * or accuracy.
 *
 * @{
 * @file
 * @brief       Peripheral hardware internal temperature interface
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_TEMP_H
#define PERIPH_TEMP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define default internal temperature type identifier
 */
#ifndef HAVE_INT_TEMP_T
typedef unsigned int int_temp_t;
#endif

/**
 * @brief   Initialize the internal temperature sensor
 *
 * Some platforms will required an initialization as they use an analog sensor
 * or specific process.
 * 
 * @param[in] temp  Temperature sensor to initialize
 */
void temp_init(int_temp_t temp);

/**
 * @brief   Read internal temperature from device
 *
 * The read function should power on the internal MCU peripheral, read the given
 * temperature and then put the peripheral back to sleep.
 *
 * @param[in] temp  Temperature sensor to read temperature from
 * 
 * @return          Given temperature in °C
 */
int8_t temp_read(int_temp_t temp);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_TEMP_H */
/** @} */