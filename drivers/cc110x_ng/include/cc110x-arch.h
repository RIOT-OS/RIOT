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

/**
 * @file
 * @ingroup		LPC2387
 * @brief		CC1100 LPC2387 dependend functions
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author		Heiko Will <hwill@inf.fu-berlin.de>
 * @version     $Revision: 1775 $
 *
 * @note    	$Id: arch_cc110x.h 1775 2010-01-26 09:37:03Z hillebra $
 */

#include <stdint.h>

uint8_t cc110x_txrx(uint8_t c);

void cc110x_gdo0_enable(void);
void cc110x_gdo0_disable(void);
void cc110x_gdo2_enable(void);
void cc110x_gdo2_disable(void);
void cc110x_init_interrupts(void);

void cc110x_before_send(void);
void cc110x_after_send(void);
