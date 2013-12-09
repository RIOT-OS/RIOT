/******************************************************************************
Copyright 2008-2009, Freie Universitaet Berlin (FUB). All rights reserved.

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

#ifndef __BOARD_H
#define __BOARD_H

/**
 * @ingroup		pttu
 * @{
 */

/**
 * @file
 * @brief		PTTU Board
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author		Kaspar Schleiser <kaspar@schleiser.de>
 * @version     $Revision$
 *
 * @note		$Id: board.h 664 2009-02-19 10:54:44Z baar $
 */

#include <lpc2387.h>
#include <cpu-conf.h>

#define VICIntEnClear VICIntEnClr

void init_clks1(void);
void init_clks2(void);
void bl_init_clks(void);

typedef uint8_t radio_packet_length_t;

/** @} */
#endif // __BOARD_H
