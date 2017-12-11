/*
 * Copyright 2017, RWTH-Aachen, Steffen Robertz, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_shtc1
 * @{
 *
 * @file
 * @brief       Device driver implementation for the Sensirion SHTC1 temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */

#include "shtc1.h"
#include "shtc1_regs.h"
#define ENABLE_DEBUG	(0)
#include "debug.h"
#include "assert.h"


/*
 * does a crc check and returns 0 for passed and -1 for failed
 */
static int8_t _check_crc(uint8_t *rec_values, uint8_t right_crc)
{
	uint8_t crc = 0xFF;
	uint8_t current_byte;
	uint8_t bit;

	for (current_byte = 0; current_byte < 2; current_byte++)
	{
		crc ^= (rec_values[current_byte]);
		for (bit = 8; bit > 0; bit--)
		{
			if (crc & 0x80) {
				crc = (crc << 1) ^ SHTC1_CRC;
			}
            else {
				crc = (crc << 1);
            }
		}
	}
	return !(crc == right_crc);
}

int8_t shtc1_init(shtc1_t* const dev, const shtc1_params_t* params)
{
    /* check for a valid device descriptor and parameters */
    assert(dev && params);
    /* copy settings into the device descriptor */
    dev->bus = params->bus;
    dev->addr = params->addr;
	dev->crc = params->crc;
    i2c_acquire(dev->bus);
    if (i2c_init_master(dev->bus, I2C_SPEED_FAST)) {
		 return SHTC1_ERROR;
	}
    i2c_release(dev->bus);
    /* Verify the connection by reading the SHTC1's ID and checking its value */
    if (shtc1_id(dev) != SHTC1_OK || ((dev->values.id&0x3F) !=  SHTC1_ID)) {
        return SHTC1_ERROR;
    }
	return SHTC1_OK;
}

int8_t shtc1_measure(shtc1_t* const dev)
{
    /* Build and issue the measurement command */
	uint8_t cmd[] = { SHTC1_MEASURE_CLOCK_STRETCHING_TEMP_HIGH, SHTC1_MEASURE_CLOCK_STRETCHING_TEMP_LOW };
	i2c_acquire(dev->bus);
    i2c_write_bytes(dev->bus, dev->addr, cmd, 2);
    /* Receive the measurement */
	uint8_t received[6];
	i2c_read_bytes(dev->bus, dev->addr, received, 6);
    i2c_release(dev->bus);
    /* get 16bit values and check crc */
	uint16_t temp_f = ((received[0]<<8)|received[1]);
	uint16_t abs_humidity = ((received[3]<<8)|received[4]);
	if (dev->crc) {
		if (!((_check_crc(&received[0], received[2]) == 0) && (_check_crc(&received[3], received[5]) == 0))) {
			/* crc check failed */
			DEBUG("CRC Error");
			return SHTC1_ERROR;
		}
		DEBUG("CRC Passed! \n");
	}
    /* calculate the relative humidity and convert the temperature to °C */
	dev->values.temp = (175.0*temp_f/65536)-45;
	dev->values.rel_humidity = 100*(abs_humidity/65536.0);

	return SHTC1_OK;
}

int8_t shtc1_id(shtc1_t* const dev)
{
    /* Build and send measurement command */
	uint8_t data[] = { SHTC1_COMMAND_ID_HIGH, SHTC1_COMMAND_ID_LOW };
	int8_t check = 0;
    i2c_acquire(dev->bus);
	check = i2c_write_bytes(dev->bus, dev->addr, data, 2);
	/* receive ID and check if the send and receive commands were successfull */
	check += i2c_read_bytes(dev->bus, dev->addr, data, 2);
    i2c_release(dev->bus);
	if (check != 4) {
		/* error occured */
		return SHTC1_ERROR;
	}
    /* Save ID in device descriptor */
	dev->values.id = (data[0]<<8)|data[1];
	return SHTC1_OK;
}

int8_t shtc1_reset(const shtc1_t* const dev)
{
    /* Build and issue the reset command */
	uint8_t data[] = { SHTC1_COMMAND_RESET_HIGH, SHTC1_COMMAND_RESET_LOW };
	int8_t check = 0;
    i2c_acquire(dev->bus);
	check = i2c_write_bytes(dev->bus, dev->addr, data, 2);
    i2c_release(dev->bus);
	if (check != 2) {
		/* error occured */
		return SHTC1_ERROR;
	}
	return SHTC1_OK;
}
