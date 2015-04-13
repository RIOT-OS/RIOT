/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Default i2c application 
 *
 * @author      Rane Balslev <ranebalslev@gmail.com>
 *
 * @}
 */

#include <stdio.h>
//#include "periph/i2c.h"
#include "SHT2x.h"
#include "hwtimer.h" 
 
//#include "lm75a-temp-sensor.h"



int main(void)
{
	printf(" :::: I2C Test Application v0.1 ::::\n" );

	if(i2c_init_master(I2C_0, I2C_SPEED_NORMAL) == 0)//!lm75A_init(0, NULL, NULL))//i2c_init_master(I2C_0, I2C_SPEED_NORMAL) == 0)
	{
		printf("I2C Init Master Success!\n" );
	}
	else
	{
		printf("I2C Init Master Error!\n" );
		while(1){};
	}
	
	printf("I2C start writing\n" );
	//char wdata[1];
	//char rdata[1];
	//uint8_t addr = 0x40; 
	//wdata[0] = 0xE3;
	//wdata[1] = 0xAA;
	uint8_t error = 0;

		//while(i2c_write_bytes(I2C_0, addr, wdata, 1) != 1);
		//while(i2c_read_bytes(I2C_0, addr, rdata, 1) != 1);

	uint8_t m[2];
	uint16_t result = 0;
	SHT2x_Init();
	//SHT2x_SoftReset();
	//hwtimer_wait(HWTIMER_TICKS_TO_US(16000));
	//error = SHT2x_MeasureHM(TEMP, m);

	//error = SHT2x_MeasurePoll(TEMP, m);
	result = read_temperature();// ((uint16_t)m[0] << 8) | m[1];

	printf("I2C data is getting ready!\n" );
	printf("\n\n\tData: %d\n Error: %d\n", result, error );
	printf("\n\n\tResult: %f\n", SHT2x_CalcTemperatureC(result) );
	//SHT2x_CalcTemperatureC((uint16_t)m[0])
	//i2c_write_bytes(I2C_0, 0x01, wdata, 1);
	//printf("I2C i2c_write_bytes DONE\n" );
	//printf("I2C start reading\n" );
	//printf("I2C i2c_read_bytes\n" );
	//data = lm75A_get_over_temperature();
	
    //printf("I2C i2c_read_bytes DONE\n" );

  	
  	
    while(1)
    { 		

    }
    
    return 0;
}

