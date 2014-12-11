/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_srf02 SRF02 ultrasonic range sensor
 * @ingroup     drivers
 * @brief       Driver for the SRF02 ultrasonic range sensor
 *
 * The connection between the MCU and the SRF02 is based on the i2c-interface.
 *
 * @{
 *
 * @file        srf02.h
 * @brief       Driver definitions for the SRF02 ultrasonic ranger.
 *
 * The connection between the SRF02 and the MCU is based on the i2c interface.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */

#ifndef SRF02_H_
#define SRF02_H_


#include <stdint.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief The sensors default I2C address */
#define SRF02_DEFAULT_ADDR                  112

/** @brief The command register which defines measurement mode */
#define SRF02_COMMAND_REG                   0x0

/** @brief The upper measurement byte */
#define SRF02_RANGE_HIGH_BYTE               0x2

/** @brief The lower measurement byte */
#define SRF02_RANGE_LOW_BYTE                0x3

/**
 * @brief Device descriptor for SRF02 sensors
 */
typedef struct {
    i2c_t i2c;               /**< I2C device the sensor is connected to */
    uint8_t addr;            /**< I2C bus address of the sensor */
} srf02_t;

/**
 * @brief Possible measurement modes for the SRF02 sensor
 */
typedef enum {
    SRF02_MODE_REAL_INCH = 0x50,                   /**< result in inches */
    SRF02_MODE_REAL_CM =   0x51,                   /**< result in centimeters */
    SRF02_REAL_RANGING_MODE_MICRO_SEC = 0x52,      /**< result in microseconds */
    SRF02_FAKE_RANGING_MODE_INCH =      0x56,      /**< synchronous measurement in inches */
    SRF02_FAKE_RANGING_MODE_CM =        0x57,      /**< synchronous measurement in centimeters */
    SRF02_FAKE_RANGING_MODE_MICRO_SEC = 0x58       /**< synchronous measurement in microseconds */
}srf02_mode_t;

/**
 * @brief       Initialize the SRF02 ultrasonic sensor
 *
 * @param[in] dev           device descriptor of an SRF02 sensor
 * @param[in] i2c           I2C device the sensor is connected to
 * @param[in] addr          I2C address of the sensor
 * @param[in] speed         I2C speed mode
 *
 * @return                  0 on successful initialization
 * @return                  -1 on undefined device given
 * @return                  -2 on unsupported speed value
 */
int srf02_init(srf02_t *dev, i2c_t i2c, uint8_t addr, i2c_speed_t speed);

/**
 * @brief       Get the distance measured from the SRF02 ultrasonic sensor
 *              The result of a ranging can be returned in inches,
 *              centimeters or microseconds
 * @param[in] dev           device descriptor of an SRF02 sensor
 * @param[in] mode          there are three real ranging modes, which return
 *                          the result in inches, centimeters or microseconds.
 *                          Another set of three fake ranging modes do the same
 *                          but without transmitting the burst
 *
 * @return                  -1 on undefined device given
 * @return the ranging result in inches, centimeters or microseconds. In the
 *         case of the fake ranging mode a zero value is returned. UINT16_MAX
 *         is returned if write/read action from the i2c-interface is failed.
 *
 */
uint16_t srf02_get_distance(srf02_t *dev, srf02_mode_t mode);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SRF02_H_ */
