/*
 * sarf02-ultrasonic-sensor.c - Driver for the SRF02 ultrasonic sensor using the LPC2387 chip.
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license, See the file LICENSE for more details.
 */

/**
 * @file
 * @internal
 * @brief		Driver for the SRF02 ultrasonic ranger using the LPC2387 chip.
 * 				The connection between the LPC2387 and the SRF08 is based on the i2c-interface.
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note		$Id: sarf02-ultrasonic-sensor.c 3854 2013-06-05 13:30:01Z zkasmi $
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <msg.h>
#include <vtimer.h>
#include <timex.h>
#include <thread.h>
#include "hwtimer.h"
#include "srf02-ultrasonic-sensor.h"
#include "i2c.h"


bool srf02_init(uint8_t i2c_interface, uint32_t baud_rate) {
	if (i2c_initialize(i2c_interface, (uint32_t) I2CMASTER, 0, baud_rate, NULL)
			== false) /* initialize I2C */
			{
		puts("fatal error! happened in i2cInitialize() \n");
		while (1)
			; /* Fatal error */
	} else {
		i2c_enable_pull_up_resistor(i2c_interface);
		//i2c_disable_pull_up_resistor(i2c_interface);
		return true;
	}

}



void srf02_start_ranging(void) {
	bool status = false;
	uint8_t reg_size = 1;
	uint8_t range_high_byte = 0;
	uint8_t range_low_byte = 0;
	uint32_t distance = 0;
	uint8_t rx_buff[reg_size];
	uint8_t tx_buff[reg_size];
	tx_buff[0] = SRF02_REAL_RANGING_MODE_CM;

	while (1) {
		status = i2c_write(SRF02_I2C_INTERFACE, SRF02_DEFAULT_ADDR,
				SRF02_COMMAND_REG, tx_buff, reg_size);
		if(!status){
			puts("Write the ranging command to the i2c-interface is failed");
			break;
		}
		hwtimer_wait(HWTIMER_TICKS(65000));
		status = i2c_read(SRF02_I2C_INTERFACE, SRF02_DEFAULT_ADDR,
				SRF02_RANGE_HIGH_BYTE, rx_buff, reg_size);
		if(!status){
			puts("Read the distance from the i2c-interface is failed");
			break;
		}
		range_high_byte = rx_buff[0];

		status = i2c_read(SRF02_I2C_INTERFACE, SRF02_DEFAULT_ADDR,
				SRF02_RANGE_LOW_BYTE, rx_buff, reg_size);
		range_low_byte = rx_buff[0];

		distance = (range_high_byte << 8) | range_low_byte;
		printf("distance = %lu cm\n", distance);
		//printf("%u | %u\n", range_high_byte, range_low_byte);
		hwtimer_wait(HWTIMER_TICKS(50000));
	}

	puts("The SRF02 range sampling is ended!!");
}
