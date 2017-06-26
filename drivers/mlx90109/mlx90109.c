/*
 * Copyright (C) 2017 Jonas Radtke <jonas.radtke@haw-hamburg.de> <jonas@radtke.dk>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 
 /**
 * @ingroup     drivers_MLX90109
 * @{
 *
 * @file
 * @brief       Driver for reading ISO 11784 & 11785 FDX-B Animal Tags
 *
 * @author      Jonas Radtke <jonas.radtke@haw-hamburg.de> <jonas@radtke.dk>
 *
 * @}
 */

 
#include "mlx90109.h"
#include "include/mlx90109_params.h"
#include "checksum/ucrc16.h"
#include <string.h>


int16_t mlx90109_init(mlx90109_t *dev, const mlx90109_params_t *params, gpio_cb_t cb)
{
	/* write config params to device descriptor */
	memcpy(&dev->p, params, sizeof(mlx90109_params_t));
	
	//Init data pin
	if (gpio_init( dev->p.data,GPIO_IN_PU) != 0) return MLX90109_GPIO_FAULT;
	//Init Clock Pin and ISR
	if (gpio_init_int( dev->p.clock, GPIO_IN , GPIO_RISING , cb , NULL) != 0) return MLX90109_GPIO_FAULT;
	//gpio_init_int( dev->p.clock, GPIO_IN_PU , GPIO_RISING , cb , NULL);
	gpio_irq_enable(dev->p.clock);
	
	//Init mode Pin if used
	if (dev->p.mode){
		if (gpio_init( dev->p.mode,GPIO_OUT) != 0) return MLX90109_GPIO_FAULT;	
	}
	//Init data Rate Pin if used
	if (dev->p.dataSelect){
		if (gpio_init( dev->p.dataSelect,GPIO_OUT) != 0) return MLX90109_GPIO_FAULT;
		
		if (MLX90109_PARAM_SPEED == 2000){
			gpio_write(dev->p.dataSelect, 1);
		}
		else if (MLX90109_PARAM_SPEED == 4000){
			gpio_write(dev->p.dataSelect, 0);
		}
	}
	//Init Modu Pin if used
	if (dev->p.modu){
		if (gpio_init( dev->p.modu,GPIO_OUT) != 0) return MLX90109_GPIO_FAULT;
		
		if (MLX90109_PARAM_CODE == 1){
			gpio_write(dev->p.modu, 1);
		}
		else if (MLX90109_PARAM_CODE == 2){
			gpio_write(dev->p.modu, 0);
		}
	
	}

	return MLX90109_OK;
	
}

int16_t mlx90109_format_fdxb(mlx90109_t *dev, mlx90109_fdxb_tagdata *tag)
{
	uint8_t i=0;
	uint8_t k=0;
	uint16_t crc = 0;
	
	dev->counter = 0;
	dev->counter_header = 0;
	
	// Data for Checksum
	for (k=0; k<=7; k++)
	{
		tag->checksumData[k] = 0;
		for (i=(k*9); i<=7+(k*9); i++)
		{
			tag->checksumData[k] |= (dev->data[i] << (i-(k*9)));
		}
	}

	// Checksum format
	tag->checksumArr[0] = 0; tag->checksumArr[1] = 0;
	for (i=72; i<=79; i++)
	{
		tag->checksumArr[0] |= (dev->data[i] << (i-72));
	}
	
	for (i=81; i<=88; i++)
	{
		tag->checksumArr[1] |= (dev->data[i] << (i-81));
	}
	
	tag->checksum16 = 0;
	tag->checksum16 = tag->checksum16 | tag->checksumArr[0] | (uint16_t)tag->checksumArr[1] << 8;
	// Checksum calculaton
	crc = ucrc16_calc_le(&tag->checksumData[0], sizeof(tag->checksumData), 0x8408 , 0x0000);
	
	if ((tag->checksum16 != crc))
	{
		tag->tagId = 0;
		return MLX90109_CRC_NOT_OK;
	}
	
	// Tag ID format
	tag->tagId = 0;
	tag->tagId = tag->tagId | (uint64_t)tag->checksumData[0] | (uint64_t)tag->checksumData[1] << 8 | (uint64_t)tag->checksumData[2] << 16 | (uint64_t)tag->checksumData[3] << 24 | (uint64_t)(tag->checksumData[4] & 0x3F) << 32 ;
	
	// Tag Countrycode format
	tag->countryCode = 0;
	tag->countryCode = tag->countryCode | ((uint16_t)tag->checksumData[5] << 2) | ((tag->checksumData[4] & 0xC0) >> 6);
	
	// Tag Data Block used and Tag for Animal Identification
	tag->dataBlock = 0;
	tag->dataBlock = tag->checksumData[6] & 0x01;
	tag->animalTag = 0;
	tag->animalTag = (tag->checksumData[7] & 0x80) >> 7;
	
	// Datablock for additional data and individual application
	for (i=90; i<=97; i++)
	{
		tag->dataB[0] |= (dev->data[i] << (i-90));
	}
	
	for (i=99; i<=106; i++)
	{
		tag->dataB[1] |= (dev->data[i] << (i-99));
	}
	
	for (i=108; i<=115; i++)
	{
		tag->dataB[2] |= (dev->data[i] << (i-108));
	}
	
	return MLX90109_OK;
}


int16_t mlx90109_read_fdxb(mlx90109_t *dev)
{
	// Detect "1"
	if((gpio_read(dev->p.data) > 0)&&(dev->counter_header==11))
	{
		dev->data[dev->counter]=1;
		dev->counter++;
	}
	// Detect "0"
	if ((!(gpio_read(dev->p.data)))&&(dev->counter_header==11))
	{
		dev->data[dev->counter]=0;
		dev->counter++;
	}
	
	// Detect Header (10000000000 	  11bit Header)
	if ((!(gpio_read(dev->p.data)))&&(dev->counter_header<10))
	{
		dev->counter_header++;
	}
	
	if ((gpio_read(dev->p.data) > 0)&&(dev->counter_header<10))
	{
		dev->counter_header=0;
		dev->counter = 0;
	}
	
	if ((gpio_read(dev->p.data) > 0) && (dev->counter_header == 10))
	{
		dev->counter_header++;
	}

	// Data complete after 127 bit
	if ( dev->counter > 127)
	{
		dev->counter = 0;
		dev->counter_header = 0;
		return MLX90109_DATA_OK;	
	}
	else 
	{
		return MLX90109_OK;
	}
}