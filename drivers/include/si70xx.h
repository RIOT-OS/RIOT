/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_si70xx Si70xx
 * @ingroup     drivers
 * @brief       Driver for the Si7006/13/20/21 temperature and humidity sensor.
 * @{
 *
 * @file
 * @brief       Interface definition of the Si70xx driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef SI70XX_H_
#define SI70XX_H_

#include <stdint.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Si70xx chip addresses.
 */
#define SI70XX_ADDRESS_SI7006       (0x80)
#define SI70XX_ADDRESS_SI7013       (0x80)
#define SI70XX_ADDRESS_SI7013_ALT   (0x81)
#define SI70XX_ADDRESS_SI7020       (0x80)
#define SI70XX_ADDRESS_SI7021       (0x80)

/**
 * @name Si70xx device commands
 * @{
 */
#define SI70XX_MEASURE_RH_HOLD      (0xE5)
#define SI70XX_MEASURE_RH           (0xF5)
#define SI70XX_MEASURE_TEMP_HOLD    (0xE3)
#define SI70XX_MEASURE_TEMP         (0xF3)
#define SI70XX_MEASURE_TEMP_PREV    (0xE0)
#define SI70XX_RESET                (0xFE)
#define SI70XX_WRITE_USER_REG       (0xE6)
#define SI70XX_READ_USER_REG        (0xE7)
#define SI70XX_WRITE_HEATER_REG     (0x51)
#define SI70XX_READ_HEATER_REG      (0x11)
#define SI70XX_READ_ID_FIRST_A      (0xFA)
#define SI70XX_READ_ID_FIRST_B      (0x0F)
#define SI70XX_READ_ID_SECOND_A     (0xFC)
#define SI70XX_READ_ID_SECOND_B     (0xC9)
#define SI70XX_READ_REVISION_A      (0x84)
#define SI70XX_READ_REVISION_B      (0xB8)
/** @} */

/**
 * @name Si70xx register values
 * @{
 */
#define SI70XX_ID_SI7006            (0x06)
#define SI70XX_ID_SI7013            (0x0D)
#define SI70XX_ID_SI7020            (0x14)
#define SI70XX_ID_SI7021            (0x15)

#define SI70XX_REVISION_1           (0xFF)
#define SI70XX_REVISION_2           (0x20)
/** @} */

/**
 * @brief Si70xx device descriptor
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C bus the sensors is connected to */
    uint8_t address;            /**< sensor address */
} si70xx_t;

/**
 * @brief   Test if the device id and revision number are as expected.
 *
 * @param[in] dev           device descriptor
 * @return                  zero on succesful test
 * @return                  non-zero on unsuccesfull test.
 */
int si70xx_test(si70xx_t *dev);

/**
 * @brief   Initialize and reset the sensor.
 *
 * @param[in] dev           device descriptor
 * @param[in] i2c_dev       i2c device to use
 * @param[in] address       device address (depends on the chip)
 * @return                  zero on succesful initialization.
 * @return                  non-zero on error
 */
int si70xx_init(si70xx_t *dev, i2c_t i2c_dev, uint8_t address);

/**
 * @brief   Read the relative humidity from the sensor. Uses clock streching.
 *
 * @param[in] dev           device descriptor
 * @return                  relative humidity in centi-percent (times 100)
 */
uint16_t si70xx_get_relative_humidity(si70xx_t *dev);

/**
 * @brief   Read the current temperature from the sensor. Uses clock streching.
 *
 * @param[in] dev           device descriptor
 * @return                  current temperature in centi-degrees Celsius
 *                          (times 100)
 */
int16_t si70xx_get_temperature(si70xx_t *dev);

/**
 * @brief   Read the relative humidity and temperature from the sensor. Uses
 *          clock stretching.
 *
 * @param[in] dev           device descriptor
 * @param[out] humidity     pointer to relative humidity (in centi-percent)
 * @param[out] temperature  pointer to temperature (in centi-degrees Celsius)
 */
void si70xx_get_both(si70xx_t *dev, uint16_t *humidity, int16_t *temperature);

/**
 * @brief   Read the sensor serial number.
 *
 * @param[in] dev           device descriptor
 * @return                  sensor serial number
 */
uint64_t si70xx_get_serial(si70xx_t *dev);

/**
 * @brief   Read the sensor id, to identifier the sensor variant.
 *
 * @param[in] dev           device descriptor
 * @return                  device id
 */
uint8_t si70xx_get_id(si70xx_t *dev);

/**
 * @brief   Read the firmware revision of the sensor.
 *
 * @param[in] dev           device descriptor
 * @return                  sensor firmware revision number
 */
uint8_t si70xx_get_revision(si70xx_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* SI70XX_H_ */
/** @} */
