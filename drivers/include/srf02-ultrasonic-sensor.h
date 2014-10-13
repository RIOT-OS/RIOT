/*
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    srf02 SRF02
 * @ingroup     drivers
 * @brief       Driver for the SRF02 ultrasonic range sensor
 *
 * The connection between the MCU and the SRF08 is based on the i2c-interface.
 *
 * @{
 *
 * @file        srf02-ultrasonic-sensor.h
 * @brief       Driver definitions for the SRF02 ultrasonic ranger.
 *
 * The connection between the SRF02 and the MCU is based on the i2c interface.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 */

#ifndef SRF02_ULTRASONIC_SENSOR_I2C_H_
#define SRF02_ULTRASONIC_SENSOR_I2C_H_
#include "i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/* define the SRF02 registers*/
#define SRF02_DEFAULT_ADDR                  112
#define SRF02_COMMAND_REG                   0x0
#define SRF02_RANGE_HIGH_BYTE               0x2
#define SRF02_RANGE_LOW_BYTE                0x3
#define SRF02_REAL_RANGING_MODE_INCH        0x50
#define SRF02_REAL_RANGING_MODE_CM          0x51
#define SRF02_REAL_RANGING_MODE_MICRO_SEC   0x52
#define SRF02_FAKE_RANGING_MODE_INCH        0x56
#define SRF02_FAKE_RANGING_MODE_CM          0x57
#define SRF02_FAKE_RANGING_MODE_MICRO_SEC   0x58


/* Define the used I2C Interface */
//#define SRF02_I2C_INTERFACE I2C0     // P0.27 SDA0, P0.28 SCL0
//#define SRF02_I2C_INTERFACE I2C1_0   // P0.0  SDA1, P0.1  SCL1
//#define SRF02_I2C_INTERFACE I2C1_1   // P0.19 SDA1, P0.20 SCL1
#define SRF02_I2C_INTERFACE I2C2       // P0.10 SDA2, P0.11 SCL2
#define SRF02_EXIT_MSG          0
#define SRF02_RANGING_MSG       1
#define SRF02_SLEEP_MSG         2
#define SRF02_WEAKUP_MSG        3


/**
 * @brief       Initialize the SRF02 ultrasonic sensor.
 *
 * @param[in] i2c_interface the i2c interface, several interfaces can be
 *                          selected: i2c0, i2c1 and i2c2.
 * @param[in] baud_rate     the baud rate.
 *
 * @return true if the SRF02 is successfully initialized, otherwise false.
 */
bool srf02_init(uint8_t i2c_interface, uint32_t baud_rate);



/**
 * @brief       Get the distance measured from the SRF02 ultrasonic sensor.
 *              The result of a ranging can be returned in inches,
 *              centimeters or microseconds
 *
 * @param[in] ranging_mode  there are three real ranging modes, which return
 *                          the result in inches, centimeters or microseconds.
 *                          Another set of three fake ranging modes do the same
 *                          but without transmitting the burst.
 *
 * @return the ranging result in inches, centimeters or microseconds. In the
 *         case of the fake ranging mode a zero value is returned. UINT32_MAX
 *         is returned if write/read action from the i2c-interface is failed.
 *
 */
uint32_t srf02_get_distance(uint8_t ranging_mode);

/**
 * @brief       Start a continuous sampling of the distance measures.
 *              This function prints the distance values over the rs232
 *              interface.
 *
 * @param[i] ranging_mode   there are three real ranging modes, which return
 *                          the result in inches, centimeters or microseconds.
 *                          Another set of three fake ranging modes do the same
 *                          but without transmitting the burst.
 *
 */
void srf02_start_ranging(uint16_t ranging_mode);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SRF02_ULTRASONIC_SENSOR_I2C_H_ */
