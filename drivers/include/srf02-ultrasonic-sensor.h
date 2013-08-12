/*
 * sarf02-ultrasonic-sensor.h - Definitions for the SRF02 ultrasonic ranger based on the i2c interface.
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license, See the file LICENSE for more details.
 */

/**
 * @file
 * @internal
 * @brief		Driver definitions for the SRF02 ultrasonic ranger using the LPC2387 chip.
 * 				The connection between the SRF02 and the LPC2387 is based on the i2c interface.
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note		$Id: sarf02-ultrasonic-sensor.h 3854 2013-06-20 13:30:01Z zkasmi $
 *
 */

#ifndef SRF02_ULTRASONIC_SENSOR_I2C_H_
#define SRF02_ULTRASONIC_SENSOR_I2C_H_
#include "i2c.h"


/* define the SRF02 registers*/
#define SRF02_DEFAULT_ADDR 			112
#define SRF02_COMMAND_REG			0x0
#define SRF02_RANGE_HIGH_BYTE		0x2
#define SRF02_RANGE_LOW_BYTE		0x3
#define SRF02_REAL_RANGING_MODE_CM	0x51


/* Define the used I2C Interface */
//#define SRF02_I2C_INTERFACE I2C0		// P0.27 SDA0, P0.28 SCL0
//#define SRF02_I2C_INTERFACE I2C1_0		// P0.0  SDA1, P0.1  SCL1
//#define SRF02_I2C_INTERFACE I2C1_1		// P0.19 SDA1, P0.20 SCL1
#define SRF02_I2C_INTERFACE I2C2	// P0.10 SDA2, P0.11 SCL2

#define SRF02_EXIT_MSG		0
#define SRF02_RANGING_MSG 	1
#define SRF02_SLEEP_MSG		2
#define SRF02_WEAKUP_MSG	3


/**
 * @brief	 Initialize the SRF02 ultrasonic sensor.
 *
 * @param[in] i2c_interface		the i2c interface, several interfaces can be selected: i2c0, i2c1 and i2c2.
 * @param[in] baud_rate			the baud rate.
 *
 *@return 	true if the SRF02 is successfully initialized, otherwise false.
 */
bool srf02_init(uint8_t i2c_interface, uint32_t baud_rate);


/**
 * @brief   Start a continuous sampling of the distance measures.
 *			This function prints the distance values in centimeters over the rs232 interface.
 *
 */
void srf02_start_ranging(void);


#endif /* SRF02_ULTRASONIC_SENSOR_I2C_H_ */
