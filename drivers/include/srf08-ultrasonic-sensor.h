/*
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    srf08 SRF08
 * @ingroup     drivers
 * @brief       Driver for the SRF08 ultrasonic range sensor
 *
 * The connection between the MCU and the SRF08 is based on the i2c-interface.
 *
 * @{
 *
 * @file
 * @internal
 * @brief       Driver definitions for the SRF08 ultrasonic.
 *
 * The communication between the MCU and SRF08 is via the i2c interface.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 */

#ifndef SRF08_ULTRASONIC_SENSOR_I2C_H_
#define SRF08_ULTRASONIC_SENSOR_I2C_H_

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


/** @brief Measure in inch mode */
#define SRF08_REAL_RANGING_MODE_CM      0x50

/** @brief Measure in cm mode */
#define SRF08_REAL_RANGING_MODE_CM      0x51

/** @brief Measure in micro-seconds mode */
#define SRF08_REAL_RANGING_MODE_CM      0x52


/** @brief Number of highest register */
#define SRF08_MAX_REGISTER_NUMBER       35

/** @brief Maximum number of echos from more distant objects */
#define SRF08_MAX_ECHO_NUMBER           17


/** @brief Define the used I2C Interface */
#define SRF08_I2C_INTERFACE I2C2        /* P0.10 SDA2, P0.11 SCL2 */


/**
 * @brief       Initialize the SRF08 ultrasonic sensor.
 *
 * @param[in] i2c_interface     the i2c interface, several interfaces can be
 *                              selected: i2c0, i2c1 and i2c2.
 * @param[in] baud_rate         the baud rate.
 *
 * @return true if the SRF08 is successfully initialized, otherwise false.
 *
 */
bool srf08_init(uint8_t i2c_interface, uint32_t baud_rate);


/**
 * @brief       Start a continuous distance ranging with the SRF08 ultrasonic
 *              range-finder.
 *              The ranging results are given over the RS232-Interface.
 *
 * @param[in] ranging_mode  there are three real ranging modes, which return
 *                          the result in inches, centimeters or microseconds.
 *                          Another set of three fake ranging modes do the same
 *                          but without transmitting the burst.
 *
 */
void srf08_start_ranging(uint8_t ranging_mode);


/**
 * @brief       Set the maximum range of the SRF08.
 *
 * @param[in] max_range  the adjusted maximal range is:
 *                       max_range = (max_rangex43mm) + 43mm.
 *
 */
void srf08_set_range(uint8_t max_range);


/**
 * @brief       Set the maximum of the analog stages.
 *
 * @param[in] max_gain  the maximal gain value.
 *
 */
void srf08_set_gain(uint8_t max_gain);


/**
 * @brief       Get the maximal range.
 *
 *
 * @return the maximal range value.
 */
uint8_t srf08_get_range(void);


/**
 * @brief       Get the maximal analog gain.
 *
 *
 * @return the maximal gain value.
 */
uint8_t srf08_get_gain(void);


/**
 * @brief       Get all distances measured from the SRF08 ultrasonic sensor.
 *              The results of a ranging can be returned in inches, centimeters
 *              or microseconds. The SRF08 can detect up to targets.
 *              This function prints the distance values over the rs232
 *              interface
 *
 * @param[in] range_array   a pointer to a buffer holding the ranging results.
 * @param[in] ranging_mode  there are three real ranging modes, which return
 *                          the result in inches, centimeters or microseconds.
 *                          Another set of three fake ranging modes do the same
 *                          but without transmitting the burst.
 *
 * @return -1 if the write/read action from the i2c-interface is failed,
 *            otherwise the number of the detected targets is delivered.
 */
int32_t srf08_get_distances(uint32_t *range_array, uint8_t ranging_mode);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SRF08_ULTRASONIC_SENSOR_I2C_H_ */
