/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_lis2dh12 LIS2DH12 Accelerometer
 * @ingroup     drivers_sensors
 * @brief       Driver for the STM LIS2DH12 accelerometer
 *
 * This device driver provides a minimal interface to LIS2DH12 devices. As of
 * now, it only provides very basic access to the device. The driver configures
 * the device to continuously read the acceleration data with statically
 * defined scale and rate, and with a fixed 10-bit resolution. The LIS2DH12's
 * FIFO is bypassed, so the driver might not be sufficient for use cases where
 * the complete history of readings is of interest.
 *
 * Also, the current version of the driver supports only interfacing the sensor
 * via SPI. The driver is however written in a way, that adding I2C interface
 * support is quite simple, as all bus related functions (acquire, release,
 * read, write) are cleanly separated in the code.
 *
 * @{
 * @file
 * @brief       Interface definition for the STM LIS2DH12 accelerometer
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LIS2DH12_H
#define LIS2DH12_H

#include "saul.h"
#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* I2C support is not implemented (yet), so throw an error when selected */
#ifndef MODULE_LIS2DH12_SPI
#error "LIS2DH12 error: I2C mode is not supported, yet. Use module li2dh12_spi"
#endif

/**
 * @brief   Available scale values
 */
typedef enum {
    LIS2DH12_SCALE_2G  = 0x00,      /**< +- 2g */
    LIS2DH12_SCALE_4G  = 0x10,      /**< +- 4g */
    LIS2DH12_SCALE_8G  = 0x20,      /**< +- 8g */
    LIS2DH12_SCALE_16G = 0x30,      /**< +- 16g */
} lis2dh12_scale_t;

/**
 * @brief   Available sampling rates
 *
 * @note    The device does also support some additional rates for specific low-
 *          power modes, but those are as of now not supported by this driver
 */
typedef enum {
    LIS2DH12_RATE_1HZ   = 0x17,     /**< sample with 1Hz */
    LIS2DH12_RATE_10HZ  = 0x27,     /**< sample with 10Hz */
    LIS2DH12_RATE_25HZ  = 0x37,     /**< sample with 25Hz */
    LIS2DH12_RATE_50HZ  = 0x47,     /**< sample with 50Hz */
    LIS2DH12_RATE_100HZ = 0x57,     /**< sample with 100Hz */
    LIS2DH12_RATE_200HZ = 0x67,     /**< sample with 200Hz */
    LIS2DH12_RATE_400HZ = 0x77,     /**< sample with 400Hz */
} lis2dh12_rate_t;

/**
 * @brief   LIS2DH12 configuration parameters
 */
typedef struct {
    spi_t spi;                      /**< SPI bus the device is connected to */
    gpio_t cs;                      /**< connected chip select pin */
    lis2dh12_scale_t scale;         /**< sampling sensitivity used */
    lis2dh12_rate_t rate;           /**< sampling rate used */
} lis2dh12_params_t;

/**
 * @brief   LIS2DH12 device descriptor
 */
typedef struct {
    const lis2dh12_params_t *p;     /**< device configuration */
    uint16_t comp;                  /**< scale compensation factor */
} lis2dh12_t;

/**
 * @brief   Status and error return codes
 */
enum {
    LIS2DH12_OK    =  0,            /**< everything was fine */
    LIS2DH12_NOBUS = -1,            /**< bus interface error */
    LIS2DH12_NODEV = -2,            /**< unable to talk to device */
};

/**
 * @brief   Export the SAUL interface for this driver
 */
extern const saul_driver_t lis2dh12_saul_driver;

/**
 * @brief   Initialize the given LIS2DH12 sensor device
 *
 * @param[out] dev      device descriptor
 * @param[in]  params   static device configuration
 *
 * @return  LIS2DH12_OK on success
 * @return  LIS2DH12_NOBUS on bus errors
 * @return  LIS2DH12_NODEV if no LIS2DH12 device was found on the bus
 */
int lis2dh12_init(lis2dh12_t *dev, const lis2dh12_params_t *params);

/**
 * @brief   Read acceleration data from the given device
 *
 * @param[in]  dev      device descriptor
 * @param[out] data     acceleration data in mili-g, **MUST** hold 3 values
 *
 * @return  LIS2DH12_OK on success
 * @return  LIS2DH12_NOBUS on bus error
 */
int lis2dh12_read(const lis2dh12_t *dev, int16_t *data);

/**
 * @brief   Power on the given device
 *
 * @param[in] dev       device descriptor
 *
 * @return  LIS2DH12_OK on success
 * @return  LIS2DH12_NOBUS on bus error
 */
int lis2dh12_poweron(const lis2dh12_t *dev);

/**
 * @brief   Power off the given device
 *
 * @param[in] dev       device descriptor
 *
 * @return  LIS2DH12_OK on success
 * @return  LIS2DH12_NOBUS on bus error
 */
int lis2dh12_poweroff(const lis2dh12_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* LIS2DH12_H */
/** @} */
