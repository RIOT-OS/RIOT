/*
 * Copyright (C) 2016 University of California, Berkeley
 * Copyright (C) 2016 Michael Andersen <m.andersen@cs.berkeley.edu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    drivers_at30ts74 Atmel AT30TS74 temperature sensor
 * @ingroup     drivers_sensors
 *
 * This chip is similar to the AT30TSE75x, but it does not have an EEPROM
 *
 * @{
 *
 * @file
 * @brief       Driver for the AT30TS74 ±1°C Accurate, 12-Bit Digital I2C
 *              Temperature Sensor from Atmel
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 */

#ifndef AT30TS74_H_
#define AT30TS74_H_

#include <stdint.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name AT30TS74 I2C address
 * @{
 */
#define AT30TS74_ADDR                   (0x49)
/** @} */

/**
 * @name AT30TS74 Valid bits options
 * @{
 */
typedef enum {
  AT30TS74_9BIT  = 0,
  AT30TS74_10BIT = 1,
  AT30TS74_11BIT = 2,
  AT30TS74_12BIT = 3
} at30ts74_bits_t;
/** @} */

/**
  * @brief   Device descriptor for a AT30TS74 device
  * @{
  */
typedef struct {
  i2c_t   i2c;
  uint8_t address;
  uint8_t bits;
  uint16_t samplems;
} at30ts74_t;
/** @} */

/**
 * @brief   Initialize a AT30TS74 device
 *
 * @param[out] dev          device descriptor
 * @param[in] i2c           I2C bus the device is connected to
 * @param[in] address       I2C address of the device
 * @param[in] bits          One of AT30TS74_BITS_{9..12} specifying the sample precision
 *
 * The valid address range is 0x48 - 0x4f depending on the exact model number,
 * and address pins (if present on the package variant)
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int at30ts74_init(at30ts74_t *dev, i2c_t i2c, uint8_t address, at30ts74_bits_t precision);

/**
 * @brief   Get measured temperature
 *
 * @param[in] dev           device descriptor
 * @param[out] temperature  the int32_t where the temperature will be written to.
 *                          Note that the result must be divided by 10000 to get
 *                          degrees C.
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int at30ts74_read(at30ts74_t *dev, int32_t *temperature);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* AT30TS74_H_ */
