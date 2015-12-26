/*
 * Copyright 2015 Ludwig Knüpfer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_isl29125 ISL29125 RGB light sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver for the ISL29125 RGB light sensor
 *
 * ## Description
 *
 * The device supports level conversion in 12, and 16 bit depth per
 * channel. Selecting a higher precision results in a longer
 * conversion time.
 *
 * The driver uses the @ref color_rgb_t color definition from @ref
 * color.h for value representation. It does not depend on the color
 * module however.
 *
 * ## Usage
 *
 * Examine `tests/driver_isr29125` for an exemplary application using
 * this driver.
 *
 * ## Caveats
 *
 * A GPIO pin definition has been included in the interface for future
 * compatibility only.
 * - The driver does not support SYNC mode at the moment.
 * - The driver supports polling only, i.e. interrupt mode is
 *   currently not supported.
 *
 * @{
 *
 * @file
 * @brief       Device driver interface for the ISL29125 RGB light sensor
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de
 */

#ifndef ISL29125_H
#define ISL29125_H

#include <stdint.h>

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "color.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief data type for storing lux RGB sensor readings
 */
typedef struct {
    float red;              /**< red lux value */
    float green;            /**< green lux value */
    float blue;             /**< blue lux value */
} isl29125_rgb_t;

/**
 * @brief supported operation modes of the ISL29125 sensor's AD
 * conversion
 */
typedef enum {
    ISL29125_MODE_DOWN = 0x00,      /**< ADC powered down */
    ISL29125_MODE_STANDBY = 0x04,   /**< AD conversion not performed */
    ISL29125_MODE_RGB = 0x05,       /**< RGB conversion */
    ISL29125_MODE_R = 0x02,         /**< red conversion only */
    ISL29125_MODE_G = 0x01,         /**< green conversion only */
    ISL29125_MODE_B = 0x03,         /**< blue conversion only */
    ISL29125_MODE_RG = 0x06,        /**< red and green conversion only */
    ISL29125_MODE_GB = 0x07         /**< green and blue conversion only */
} isl29125_mode_t;

/**
 * @brief supported RGB sensing range values of the ISL29125 sensor
 */
typedef enum {
    ISL29125_RANGE_375 = 0x00,      /**< range: 5.7m - 375 lux */
    ISL29125_RANGE_10K = 0x08       /**< range: 0.152 - 10,000 lux */
} isl29125_range_t;

/**
 * @brief supported color resolutions of the ISL29125 sensor's AD
 * conversion
 */
typedef enum {
    ISL29125_RESOLUTION_12 = 0x10,  /**< resolution: 12 bit */
    ISL29125_RESOLUTION_16 = 0x00   /**< resolution: 16 bit */
} isl29125_resolution_t;

/**
 * @brief Device descriptor for ISL29125 sensors
 */
typedef struct {
    i2c_t i2c;                      /**< I2C device the sensor is connected to */
    gpio_t gpio;                    /**< GPIO pin for interrupt/sync mode */
    isl29125_range_t range;         /**< sensor range */
    isl29125_resolution_t res;      /**< sensor resolution */
} isl29125_t;

/**
 * @brief initialize a new ISL29125 device
 *
 * @param[in] dev           device descriptor of an ISL29125 device
 * @param[in] i2c           I2C device the sensor is connected to
 * @param[in] gpio          GPIO pin for interrupt/sync mode (currently unused)
 * @param[in] mode          operation mode
 * @param[in] range         measurement range
 * @param[in] resolution    AD conversion resolution
 *
 * @return              0 on success
 * @return              -1 on error
 */
int isl29125_init(isl29125_t *dev, i2c_t i2c, gpio_t gpio,
                  isl29125_mode_t mode, isl29125_range_t range,
                  isl29125_resolution_t resolution);

/**
 * @brief read RGB values from device
 *
 * @param[in] dev       device descriptor of an ISL29125 device
 * @param[in] dest      pointer to lux RGB color object data is written to
 */
void isl29125_read_rgb_lux(isl29125_t *dev, isl29125_rgb_t *dest);

 /**
 * @brief read color values from device
 *
 * @param[in] dev       device descriptor of an ISL29125 device
 * @param[in] dest      pointer to RGB color object data is written to
 */
void isl29125_read_rgb_color(isl29125_t *dev, color_rgb_t *dest);

/**
 * @brief set the device's operation mode
 *
 * @param[in] dev       device descriptor of an ISL29125 device
 * @param[in] mode      operation mode
 */
void isl29125_set_mode(isl29125_t *dev, isl29125_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* ISL29125_H */
/** @} */
