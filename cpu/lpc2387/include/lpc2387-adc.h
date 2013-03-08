/******************************************************************************
Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

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

#ifndef LPC2387ADC_H_
#define LPC2387ADC_H_

/**
 * @defgroup	lpc2387_adc		LPC2387 ADC
 * @ingroup		lpc2387
 *
 * @{
 */

/**
 * @file
 * @brief		LPC2387 ADC
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @version     $Revision: 3249 $
 *
 * @note    	$Id: lpc2387-adc.h 3249 2011-03-11 09:44:46Z schmittb $
 */

#include <stdint.h>

#define ADC_NUM 				 (6)
#define ADC_OFFSET			  (0x10)
#define ADC_INDEX				 (4)

#define ADC_DONE		(0x80000000)
#define ADC_OVERRUN		(0x40000000)

/**
 * @brief	Initialize ADC.
 */
void adc_init(void);
void adc_init_1(void);
void adc_init_2(void);

/**
 * @brief	Read ADC value of selected channel.
 *
 * Valid channel numbers are from 0 to 2.
 *
 * @return	ADC value of selected channel.
 */
uint16_t adc_read(uint8_t channel);

/** @} */
#endif /* LPC2387ADC_H_ */
