/*
 * Copyright 2017, RWTH-Aachen, Steffen Robertz, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "shtc1.h"

#define ENABLE_DEBUG	(0)
#include "debug.h"


/*
 * does a crc check and returns 0 for passed and -1 for failed
 */
int8_t _check_crc(uint8_t *rec_values, uint8_t right_crc)
{
	uint8_t crc = 0xFF;
	uint8_t current_byte;
	uint8_t bit;

	for (current_byte = 0; current_byte < 2; current_byte++)
	{
		crc ^= (rec_values[current_byte]);
		for (bit = 8; bit > 0; bit--)
		{
			if (crc & 0x80)
				crc = (crc << 1) ^ 0x31;
			else
				crc = (crc << 1);
		}
	}
	return !(crc == right_crc);
}

int8_t shtc1_init(i2c_t dev)
{
	int8_t error = i2c_init_master(dev, I2C_SPEED_FAST);
	if(!error) {
		 return 0;
	}else {
		return error;
	}
}

int8_t shtc1_measure(i2c_t dev, crc_type_t crc, shtc1_values_t* received_values)
{
	uint8_t data[] = { 0x7C, 0xA2 };
	i2c_write_bytes(dev, 0x70, data, 2);
	uint8_t received[6];
	i2c_read_bytes(dev, 0x70, received, 6);
	uint16_t temp_f = ((received[0]<<8)|received[1]);
	uint16_t abs_humidity = ((received[3]<<8)|received[4]);
	if(crc){
		if(!((_check_crc(&received[0], received[2]) == 0) && (_check_crc(&received[3], received[5]) == 0))){
			/* crc check failed */
			DEBUG("CRC Error");
			return -1;
		}
		DEBUG("CRC Passed! \n");
	}
	received_values->temp = (175.0*temp_f/65536)-45;
	received_values->rel_humidity = 100*(abs_humidity/65536.0);

	return 0;
}

int8_t shtc1_id(i2c_t dev, uint16_t *id)
{
	uint8_t data[] = { 0xEF, 0xC8 };
	int8_t check = 0;
	check = i2c_write_bytes(dev, 0x70, data, 2);
	uint8_t received[2];
	check += i2c_read_bytes(dev, 0x70, received, 2);
	if(check != 4){
		/* error occured return -1 */
		return -1;
	}
	*id = (received[0]<<8)|received[1];
	return 0;
}

int8_t shtc1_reset(i2c_t dev)
{
	uint8_t data[] = { 0x80, 0x5D };
	int8_t check = 0;
	check = i2c_write_bytes(dev, 0x70, data, 2);
	if(check != 2){
		/* error occured return -1 */
		return -1;
	}
	return 0;
}
