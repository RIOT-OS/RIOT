/*
 * Copyright 2017, RWTH Aachen. All rights reserved.
 *	@author Steffen Robertz <steffen.robertz@rwth-aachen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief driver for shtc1
 *
 * This Driver implements the shtc1 Humidity and Temperature Sensor
 * using I2C
 */

#include "periph/i2c.h"
#include "debug.h"
#include <stdint.h>

/* I2C Speeds defined in /drivers/include/periph/i2c.h I2C_SPEED_FAST = 400Hz */
#define SHTC1_SPEED		I2C_SPEED_FAST

typedef enum{
	CRC_DISABLED = 0,
	CRC_ENABLED
}crc_type;

typedef struct {
	float temp;
	float rel_humidity;
}shtc1_values_t;


/**
 * @brief initializes the sensor and i2c
 *
 * @param[in] dev			i2c device to be used
 *
 * @return					0 on a working initialization
 * @return                  -1 on undefined i2c device given in periph_conf
 * @return                  -2 on unsupported speed value
 */
int8_t shtc1_init(i2c_t dev);

/**
 * @brief reads temperature and humidity values
 *
 * @param[in] dev		The I2C Device
 * @param[out] received_values		the received values are going to be saved here
 *
 * @return					0 on a verified and working measurement
 * @return					-1 on a checksum error
 */
int8_t shtc1_measure(i2c_t dev, crc_type crc, shtc1_values_t* received_values);

/**
 * @brief reads out id
 *
 *When working correctly id should equal xxxx'xxxx'xx00'0111
 *Where x is unspecified
 *
 * @param[in] dev		The I2C Device
 * @param[out] id		contains the id read from i2c
 *
 * @return 					0 on everything done
 * @return					-1 on error occured
 */
int8_t shtc1_id(i2c_t dev, uint16_t *id);

/**
 * @brief resets sensor
 *
 *This will reset all internal state machines and reload calibration data from the memory
 *
 * @param[in] dev		The I2C Device
 *
 * @return 					0 on everything done
 * @return					-1 on error occured
 */
int8_t shtc1_reset(i2c_t dev);

