/******************************************************************************
Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of FeuerWare.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
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

#ifndef HALPLATFORM_H_
#define HALPLATFORM_H_

/**
 * @ingroup		msba2
 * @{
 */

/**
 * @file
 * @brief
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author		baar
 * @version     $Revision$
 *
 * @note		$Id$
 */

#include "vdevice.h"
#include "device-gpio.h"
#include "device-rs232.h"
#include "device-serial.h"

VDEVICE_NAME(vdevice_gpio, gpio_led_green);
VDEVICE_NAME(vdevice_gpio, gpio_led_red);
VDEVICE_NAME(vdevice_gpio, gpio_led_usb);

/**
 * @var	tty0
 * @brief	RS232 TTY0 device on UART0
 */
VDEVICE_NAME(vdevice_rs232, tty0);

/**
 * @var	console0
 * @brief	console device on tty0
 */
VDEVICE_NAME(vdevice_serial, console0);

/** @} */
#endif /* HALPLATFORM_H_ */
