/*
 * sarf08-ultrasonic-sensor.h - Definitions for the SRF02 ultrasonic ranger using the LPC2387 chip.
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license, 
 * See the file LICENSE for more details.
 */

/**
 * @file
 * @internal
 * @brief	Driver definitions for the SRF08 ultrasonic ranger using the LPC2387 chip. The communication
 * 		between the LPC23 chip and SRF08 is via the i2c interface
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author	Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note	$Id: sarf08-ultrasonic-sensor.h 3854 2013-06-21 12:30:01Z zkasmi $
 *
 */

#ifndef SRF08_ULTRASONIC_SENSOR_I2C_H_
#define SRF08_ULTRASONIC_SENSOR_I2C_H_

/* define the SRF02 registers*/
#define SRF08_DEFAULT_ADDR 		112
#define SRF08_COMMAND_REG		0x0
#define SRF08_RANGE_HIGH_BYTE		0x2
#define SRF08_RANGE_LOW_BYTE		0x3
#define SRF08_REAL_RANGING_MODE_CM	0x51
#define SRF08_RANGE_REG			0x2
#define SRF08_GAIN_REG			0x1

#define MAX_REGISTER_NUMBER		35

/* Define the used I2C Interface */
//#define I2C_INTERFACE I2C0		// P0.27 SDA0, P0.28 SCL0
//#define I2C_INTERFACE I2C1_0		// P0.0  SDA1, P0.1  SCL1
//#define I2C_INTERFACE I2C1_1		// P0.19 SDA1, P0.20 SCL1
#define SRF08_I2C_INTERFACE I2C2	// P0.10 SDA2, P0.11 SCL2


/**
 * @brief	Initialize the SRF08 ultrasonic sensor.
 *
 * @param[in] i2c_interface	the i2c interface, several interfaces can be selected: i2c0, i2c1 and i2c2.
 * @param[in] baud_rate		the baud rate.
 *
 *@return true if the SRF08 is successfully initialized, otherwise false.
 *
 */

bool srf08_init(uint8_t i2c_interface, uint32_t baud_rate);


/**
 * @brief	Start a continuous distance ranging with the SRF08 ultrasonic range-finder.
 *		The ranging results are given in centimeters over the RS232-Interface.
 *
 */
void srf08_start_ranging(void);


/**
 * @brief	Set the maximum range of the SRF08.
 *
 * @param[in] max_range	the adjusted maximal range is: max_range = (max_rangex43mm) + 43mm.
 *
 */
void srf08_set_range(uint8_t max_range);



/**
 * @brief	Set the maximum of the analog stages.
 *
 * @param[in] max_gain	the maximal gain value.
 *
 */
void srf08_set_gain(uint8_t max_gain);


/**
 * @brief	Get the maximal range.
 *
 *
 * @return the maximal range value.
 */
uint8_t srf08_get_range(void);


/**
 * @brief Get the maximal analog gain.
 *
 *
 * @return the maximal gain value.
 */
uint8_t srf08_get_gain(void);

#endif /* SRF08_ULTRASONIC_SENSOR_I2C_H_ */
