/*
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_srf02 SRF02
 * @ingroup     drivers_sensors
 * @brief       Driver for the SRF02 ultrasonic range sensor
 * @{
 *
 * @file
 * @brief       Driver definitions for the SRF02 ultrasonic ranger.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SRF02_H_
#define SRF02_H_

#include <stdint.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default I2C address of SRF02 sensors
 */
#define SRF02_DEFAULT_ADDR                  112

/**
 * @brief   Device descriptor for SRF02 sensors
 */
typedef struct {
    i2c_t i2c;               /**< I2C device the sensor is connected to */
    uint8_t addr;            /**< I2C bus address of the sensor */
} srf02_t;

/**
 * @brief   Possible measurement modes of the SRF02 sensor
 */
typedef enum {
    SRF02_MODE_REAL_INCH    = 0x50,     /**< result in inches */
    SRF02_MODE_REAL_CM      = 0x51,     /**< result in centimeters */
    SRF02_MODE_REAL_MS      = 0x52,     /**< result in microseconds */
    SRF02_MODE_FAKE_INCH    = 0x56,     /**< result in inches (no pulse send) */
    SRF02_MODE_FAKE_CM      = 0x57,     /**< result in cm (no pulse send) */
    SRF02_MODE_FAKE_MS      = 0x58      /**< result in ms (no pulse send) */
} srf02_mode_t;

/**
 * @brief   Initialize the SRF02 ultrasonic sensor
 *
 * @param[in] dev           device descriptor of an SRF02 sensor
 * @param[in] i2c           I2C device the sensor is connected to
 * @param[in] addr          I2C address of the sensor
 *
 * @return                  0 on successful initialization
 * @return                  -1 on error
 */
int srf02_init(srf02_t *dev, i2c_t i2c, uint8_t addr);

/**
 * @brief   Get the distance measured from the SRF02 ultrasonic sensor
 *
 * The result of the ranging operation is returned in inches, centimeters or
 * microseconds - depending on the given @p mode parameter.
 *
 * @param[in] dev           device descriptor of an SRF02 sensor
 * @param[in] mode          there are three real ranging modes, which return
 *                          the result in inches, centimeters or microseconds.
 *                          Another set of three fake ranging modes do the same
 *                          but without transmitting the burst
 *
 * @return  the ranging result in inches, centimeters or microseconds
 *
 */
uint16_t srf02_get_distance(srf02_t *dev, srf02_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* SRF02_H_ */
/** @} */
