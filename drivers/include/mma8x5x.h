/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mma8x5x MMA8x5x Accelerometer
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the Freescale MMA8x5x 3-Axis accelerometer.
 *
 * The driver will initialize the accelerometer for best resolution. After the
 * initialization the accelerometer will make measurements at periodic times.
 * The measurements period and scale range can be determined by accelerometer
 * initialization.
 * This driver only implements basic functionality (i.e. no support
 * for external interrupt pins).
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Interface definition for the MMA8x5x accelerometer driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef MMA8X5X_H
#define MMA8X5X_H

#include <stdint.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_mma8x5x_config     MMA8x5x Accelerometer driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief   Accelerometer Default Address
 *
 * The address value depends on the state of SA0 Address Pin in the case of MMA845x series.
 * For more details refer Table 1 in datasheet (MMA8451Q/MMA8452Q/MMA8453Q)
 */
#ifndef CONFIG_MMA8X5X_I2C_ADDRESS
#define CONFIG_MMA8X5X_I2C_ADDRESS           0x1D
#endif
/** @} */

/**
 * @brief   Devices supported by this driver
 */
enum {
    MMA8X5X_TYPE_MMA8652 = 0x4a,    /**< MMA8652 */
    MMA8X5X_TYPE_MMA8653 = 0x5a,    /**< MMA8653 */
    MMA8X5X_TYPE_MMA8451 = 0x1a,    /**< MMA8451 */
    MMA8X5X_TYPE_MMA8452 = 0x2a,    /**< MMA8452 */
    MMA8X5X_TYPE_MMA8453 = 0x3a     /**< MMA8453 */
};

/**
 * @brief   Available sampling rates
 */
enum {
    MMA8X5X_RATE_800HZ = (0 << 3),  /**< 800 Hz Output Data Rate in WAKE mode */
    MMA8X5X_RATE_400HZ = (1 << 3),  /**< 400 Hz Output Data Rate in WAKE mode */
    MMA8X5X_RATE_200HZ = (2 << 3),  /**< 200 Hz Output Data Rate in WAKE mode */
    MMA8X5X_RATE_100HZ = (3 << 3),  /**< 100 Hz Output Data Rate in WAKE mode */
    MMA8X5X_RATE_50HZ  = (4 << 3),  /**< 50 Hz Output Data Rate in WAKE mode */
    MMA8X5X_RATE_1HZ25 = (5 << 3),  /**< 12.5 Hz Output Data Rate in WAKE mode */
    MMA8X5X_RATE_6HZ25 = (6 << 3),  /**< 6.25 Hz Output Data Rate in WAKE mode */
    MMA8X5X_RATE_1HZ56 = (7 << 3)   /**< 1.56 Hz Output Data Rate in WAKE mode */
};

/**
 * @brief   Available range options
 */
enum {
    MMA8X5X_RANGE_2G = 0,           /**< +/- 2 g Full Scale Range */
    MMA8X5X_RANGE_4G = 1,           /**< +/- 4 g Full Scale Range */
    MMA8X5X_RANGE_8G = 2            /**< +/- 8 g Full Scale Range */
};

/**
 * @brief   Named return values
 */
enum {
    MMA8X5X_OK          =  0,       /**< everything was fine */
    MMA8X5X_DATA_READY  =  1,       /**< new data ready to be read */
    MMA8X5X_NOI2C       = -1,       /**< I2C communication failed */
    MMA8X5X_NODEV       = -2,       /**< no MMA8X5X device found on the bus */
    MMA8X5X_NODATA      = -3        /**< no data available */
};

/**
 * @brief   Configuration parameters
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus the device is connected to */
    uint8_t addr;               /**< I2C bus address of the device */
    uint8_t rate;               /**< sampling rate to use */
    uint8_t range;              /**< scale range to use */
    uint8_t offset[3];          /**< data offset in X, Y, and Z direction */
} mma8x5x_params_t;

/**
 * @brief   Device descriptor for MMA8x5x accelerometers
 */
typedef struct {
    mma8x5x_params_t params;    /**< device configuration parameters */
} mma8x5x_t;

/**
 * @brief   Data type for the result data
 */
typedef struct {
    int16_t x;                  /**< acceleration in X direction */
    int16_t y;                  /**< acceleration in Y direction */
    int16_t z;                  /**< acceleration in Z direction */
} mma8x5x_data_t;

/**
 * @brief   Initialize the MMA8x5x accelerometer driver.
 *
 * @param[out] dev          device descriptor of accelerometer to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  MMA8X5X_OK on success
 * @return                  MMA8X5X_NOI2C if initialization of I2C bus failed
 * @return                  MMA8X5X_NODEV if accelerometer test failed
 */
int mma8x5x_init(mma8x5x_t *dev, const mma8x5x_params_t *params);

/**
 * @brief   Set user offset correction
 *
 * Offset correction registers will be erased after accelerometer reset.
 *
 * @param[out] dev          device descriptor of accelerometer to initialize
 * @param[in]  x            offset correction value for x-axis
 * @param[in]  y            offset correction value for y-axis
 * @param[in]  z            offset correction value for z-axis
 */
void mma8x5x_set_user_offset(const mma8x5x_t *dev, int8_t x, int8_t y, int8_t z);

/**
 * @brief   Set active mode, this enables periodic measurements
 *
 * @param[out] dev          device descriptor of accelerometer to reset
 */
void mma8x5x_set_active(const mma8x5x_t *dev);

/**
 * @brief   Set standby mode.
 *
 * @param[in]  dev          device descriptor of accelerometer
 */
void mma8x5x_set_standby(const mma8x5x_t *dev);

/**
 * @brief   Check for new set of measurement data
 *
 * @param[in]  dev          device descriptor of accelerometer
 *
 * @return                  MMA8X5X_DATA_READY if new sample is ready
 * @return                  MMA8X5X_NODATA if nothing is available
 */
int mma8x5x_is_ready(const mma8x5x_t *dev);

/**
 * @brief   Read accelerometer's data
 *
 * Acceleration will be calculated as:<br>
 *     \f$ a = \frac{value \cdot 1000}{1024} \cdot mg \f$ if full scale is set
 *     to 2g<br>
 *     \f$ a = \frac{value \cdot 1000}{512} \cdot mg \f$ if full scale is set to
 *     4g<br>
 *     \f$ a = \frac{value \cdot 1000}{256} \cdot mg \f$ if full scale is set to
 *     8g<br>
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[out] data         the current acceleration data [in mg]
 */
void mma8x5x_read(const mma8x5x_t *dev, mma8x5x_data_t *data);

/**
 * @brief   Configure motion detection interrupt
 *
 * User needs to configure MCU side of the selected int pin.  mma8x5x will set
 * the pin to low on interrupt.  Before another interrupt can occur, the
 * current interrupt must be acknowledged using @p mma8x5x_ack_int().
 *
 * @param[in]   dev         device descriptor of accelerometer
 * @param[in]   int_pin     select mma8x5x int pin (1 or 2)
 * @param[in]   threshold   motion detection threshold (see datasheet)
 */
void mma8x5x_set_motiondetect(const mma8x5x_t *dev, uint8_t int_pin, uint8_t threshold);

/**
 * @brief   Acknowledge motion detection interrupt
 *
 * Acknowledges (clears) a motion detection interrupt.
 * See @ref mma8x5x_set_motiondetect().
 *
 * @warning: this does incur an I2C write, thus should not be done from within
 *           the ISR.
 *
 * @param[in]   dev         device descriptor of accelerometer
 */
void mma8x5x_ack_int(const mma8x5x_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MMA8X5X_H */
