/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ccs811 CCS811 Gas sensor (eCO2 and TVOC)
 * @ingroup     drivers_sensors
 * @brief       Device driver support for the CCS811 sensor.
 * @{
 *
 * @file
 * @brief       Device driver interface for the CCS811 sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CCS811_H
#define CCS811_H

#include "saul.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Measurements drive mode
 */
typedef enum {
    CCS811_DRIVE_MODE_0,        /**< Measurements disabled */
    CCS811_DRIVE_MODE_1,        /**< Measure every seconds */
    CCS811_DRIVE_MODE_2,        /**< Measure every 10 seconds, default value */
    CCS811_DRIVE_MODE_3,        /**< Measure every 60 seconds (low power) */
    CCS811_DRIVE_MODE_4,        /**< Constant measure, every 250ms */
} ccs811_drive_mode_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C device which is used */
    uint8_t i2c_addr;           /**< I2C address */
    gpio_t wake_pin;            /**< Wake pin */
} ccs811_params_t;

/**
 * @brief   Device descriptor for the CCS811 sensor
 */
typedef struct {
    ccs811_params_t params;     /**< Device initialization parameters */
} ccs811_t;

/**
 * @brief   Initialize the given CCS811 device
 *
 * @param[out] dev          Initialized device descriptor of CCS811 device
 * @param[in]  params       Initialization parameters
 *
 * @return                  0 on success
 * @return                  -ENODEV if not a CCS811 at given address
 * @return                  -ECANCELLED if initialization cannot continue
 */
int ccs811_init(ccs811_t *dev, const ccs811_params_t *params);

/**
 * @brief   Set measure drive mode
 *
 * @param[in] dev           Device descriptor of CCS811 device
 * @param[in] mode          The measure drive mode
 *
 * @return                  0 on success
 * @return                  -EIO if the drive could be written
 */
int ccs811_set_drive_mode(const ccs811_t *dev, ccs811_drive_mode_t mode);

/**
 * @brief   Read eCO2 and TVOC data
 *
 * @param[in]  dev          Device descriptor of CCS811 device
 * @param[out] eco2         address of eCO2 measure or NULL
 * @param[out] tvoc         address of TVOC measure or NULL
 *
 * @return                  0 on success
 * @return                  -EIO on register access error
 */
int ccs811_read_data(const ccs811_t *dev, uint16_t *eco2, uint16_t *tvoc);

/**
 * @brief   Read eCO2 measure
 *
 * @param[in]  dev          Device descriptor of CCS811 device
 * @param[out] eco2         eCO2 measure
 *
 * @return                  0 on success
 * @return                  -EIO on register access error
 */
int ccs811_read_eco2(const ccs811_t *dev, uint16_t *eco2);

/**
 * @brief   Read TVOC measure
 *
 * @param[in]  dev          Device descriptor of CCS811 device
 * @param[out] tvoc         TVOC measure
 *
 * @return                  0 on success
 * @return                  -EIO on register access error
 */
int ccs811_read_tvoc(const ccs811_t *dev, uint16_t *tvoc);

#ifdef __cplusplus
}
#endif

#endif /* CCS811_H */
/** @} */
