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

/**
 * @ingroup		msba2
 * @ingroup		ltc4150
 * @{
 */

/**
 * @file
 * @brief		LTC4150 MSB-A2 specific implemetation
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author		Heiko Will
 * @author		Michael Baar
 * @author      Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 */

#include <stdio.h>
#include "lpc2387.h"
#include "ltc4150_arch.h"
#include "gpioint.h"

void __attribute__((__no_instrument_function__)) ltc4150_disable_int(void)
{
    gpioint_set(0, BIT4, GPIOINT_DISABLE, NULL);
}

void __attribute__((__no_instrument_function__)) ltc4150_enable_int(void)
{
    gpioint_set(0, BIT4, GPIOINT_FALLING_EDGE, &ltc4150_interrupt);
}

void __attribute__((__no_instrument_function__)) ltc4150_sync_blocking(void)
{
    while (!(FIO0PIN & BIT4)) {};
}

void __attribute__((__no_instrument_function__)) ltc4150_arch_init()
{
    FIO0DIR |= BIT5;
    FIO0SET = BIT5;
}

/** @} */
