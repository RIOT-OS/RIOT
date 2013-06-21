/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

RIOT is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

#ifndef SHT11_H_
#define SHT11_H_

/**
 * @defgroup	sht11	Sensirion SHT11 Driver
 * @ingroup		drivers
 * @{
 */

/**
 * @file
 * @brief		SHT11 Device Driver
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @version     $Revision: 667 $
 *
 * @note		$Id: sht11.h 667 2009-02-19 15:06:38Z baar $
 */
#include <stdint.h>

#define SHT11_NO_ACK		(0)
#define SHT11_ACK			(1)
//adr command  r/w
#define SHT11_STATUS_REG_W	(0x06) 	//000  0011    0
#define SHT11_STATUS_REG_R	(0x07) 	//000  0011    1
#define SHT11_MEASURE_TEMP	(0x03) 	//000  0001	   1
#define SHT11_MEASURE_HUMI	(0x05) 	//000  0010	   1
#define SHT11_RESET			(0x1E) 	//000  1111	   0

/* time to wait after toggling the data line */
#define SHT11_DATA_WAIT     (HWTIMER_TICKS(1))
/* time to wait after toggling the clock line */
#define SHT11_CLK_WAIT      (HWTIMER_TICKS(1))

/* set measurement timeout to 1 second */
#define SHT11_MEASURE_TIMEOUT   (1000)

/**
 * @brief   sht11 measureable data
 */
typedef struct {
    float	temperature;    /**< temperature value */
    float	relhum;         /**< linear relative humidity */
    float	relhum_temp;    /**< temperature compensated relative humidity */
} sht11_val_t;

/**
 * @brief	SHT11 modes that can be measured
 */
typedef enum {
    TEMPERATURE = 1,
    HUMIDITY = 2
} sht11_mode_t;

/**
 * @brief	Initialize SHT11 ports
 */
void sht11_init(void);

/**
 * @brief	Read sensor
 *
 * @param value The struct to be filled with measured values
 * @param mode  Specifies type of data to be read
 *
 * @return  1 on success, 0 otherwise
 *
 * Example:
 * \code sht11_val sht11;
 * sht11_read_sensor(&sht11, HUMIDITY|TEMPERATURE);
 * printf("%-6.2f °C %5.2f %% %5.2f %%\n", sht11.temperature, sht11.relhum, sht11.relhum_temp); \endcode
 */
uint8_t sht11_read_sensor(sht11_val_t *value, sht11_mode_t mode);

/**
 * @brief   Write status register
 *
 * @param   p_value The value to write
 *
 * @return  1 on success, 0 otherwise
 */
uint8_t sht11_write_status(uint8_t *p_value);

/**
 * @brief   Read status register with checksum
 *
 * @param   p_value The read value
 * @param   p_checksum The received checksum
 *
 * return   1 on success, 0 otherwise
 */
uint8_t sht11_read_status(uint8_t *p_value, uint8_t *p_checksum);

/** @} */
#endif /*SHT11_H_*/

