/*
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_srf08 SRF08 ultrasonic range sensor
 * @ingroup     drivers
 * @brief       Driver for the SRF08 ultrasonic range sensor
 *
 * The connection between the MCU and the SRF08 is based on the i2c-interface.
 *
 * @{
 *
 * @file        srf08.h
 * @brief       Driver definitions for the SRF02 ultrasonic ranger.
 *
 * The connection between the SRF08 and the MCU is based on the i2c interface.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */

#ifndef SRF08_H_
#define SRF08_H_

#include <stdint.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief The sensors default I2C address */
#define SRF08_DEFAULT_ADDR              112

/** @brief The sensors command register (write) */
#define SRF08_COMMAND_REG               0x0

/** @brief Max. gain register (write) */
#define SRF08_GAIN_REG                  0x1

/** @brief The sensors range register (write) */
#define SRF08_RANGE_REG                 0x2

/** @brief The upper measurement byte (read) */
#define SRF08_RANGE_HIGH_BYTE           0x2

/** @brief The lower measurement byte (read)*/
#define SRF08_RANGE_LOW_BYTE            0x3

/** @brief Number of highest register */
#define SRF08_MAX_REGISTER_NUMBER       35

/** @brief Maximum number of echos from more distant objects */
#define SRF08_MAX_ECHO_NUMBER           17

/** @brief Maximum range the sensor can measure (6m) */
#define SRF08_MAX_RANGE_6M              0x8C

/** @brief Maximum gain of the sensor (1025)*/
#define SRF08_MAX_GAIN                  0x25

/**
 * @brief Device descriptor for SRF08 sensors
 */
typedef struct {
    i2c_t i2c;               /**< I2C device the sensor is connected to */
    uint8_t addr;            /**< I2C bus address of the sensor */
} srf08_t;

/**
 * @brief Possible measurement modes for the SRF08 sensor
 */
typedef enum {
    SRF08_MODE_INCH =          0x50,      /**< result in inches */
    SRF08_MODE_CM =            0x51,      /**< result in centimeters */
    SRF08_MODE_MICRO_SEC =     0x52,      /**< result in centimeters */
    SRF08_ANN_MODE_INCH =      0x53,      /**< synchronous measurement in inches */
    SRF08_ANN_MODE_CM =        0x54,      /**< synchronous measurement in centimeters */
    SRF08_ANN_MODE_MICRO_SEC = 0x55       /**< synchronous measurement in microseconds */
}srf08_mode_t;

/**
 * @brief       Initialize the SRF08 ultrasonic sensor
 *
 * @param[in] dev           device descriptor of an SRF08 sensor
 * @param[in] i2c           I2C device the sensor is connected to
 * @param[in] addr          I2C address of the sensor
 * @param[in] speed         I2C speed mode
 *
 * @return                  0 on successful initialization
 * @return                  -1 on undefined device given
 * @return                  -2 on unsupported speed value
 * @return                  -3 on max. range error
 * @return                  -4 on max. gain error
 *
 */
int srf08_init(srf08_t *dev, i2c_t i2c, uint8_t addr, i2c_speed_t speed);

/**
 * @brief       Set the maximum range of the SRF08.
 *
 * @param[in] dev           device descriptor of an SRF08 sensor
 * @param[in] max_range     the adjusted maximal range is:
 *                          max_range = (max_range x 43mm) + 43mm.
 *                          The hardware limit is 6m.
 *
 * @return                  1 on success
 * @return                  -1 on undefined device given
 *
 */
int srf08_set_max_range(srf08_t *dev, uint8_t max_range);

/**
 * @brief       Set the maximum of the analog stages.
 *
 * @ note
 * This value is just a limitation of the maximum amplification and not the actual.
 * While measuing, this value starts at its minimum and increases approx. each 75 us
 * until the maximum value is reached.
 *
 * @param[in] dev           device descriptor of an SRF08 sensor
 * @param[in] max_gain  the maximal gain value.
 *
 * @return                  1 on success
 * @return                  -1 on undefined device given
 *
 */
int srf08_set_max_gain(srf08_t *dev, uint8_t max_gain);

/**
 * @brief       Get all distances measured from the SRF08 ultrasonic sensor.
 *              The results of a ranging can be returned in inches, centimeters
 *              or microseconds. The SRF08 can detect up to 17 targets.
 *
 *
 * @param[in] dev           device descriptor of an SRF08 sensor
 * @param[in] range_array   a pointer to a buffer holding the ranging results.
 * @param[in] num_echos     number of stored measurements (echos)
 * @param[in] ranging_mode  there are three real ranging modes, which return
 *                          the result in inches, centimeters or microseconds.
 *                          Another set of three fake ranging modes do the same
 *                          but without transmitting the burst.
 *
 * @return                  number of measured echos on success
 * @return                  -1 on ranging mode errors
 * @return                  -2 if num_echos exceeds the max. number of
                               possible measurements
 * @return                  -3 if i2c read high byte failed
 * @return                  -4 if i2c read low byte failed
 *
 */
int srf08_get_distances(srf08_t *dev, uint16_t *range_array, int num_echos, srf08_mode_t ranging_mode);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SRF08_H_ */
