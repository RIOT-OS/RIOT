/******************************************************************************
Copyright 2013, Freie Universitaet Berlin (FUB). All rights reserved.

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

#ifndef SHT11BOARD_H_
#define SHT11BOARD_H_

/**
 * @ingroup     msb_430h
 * @{
 */

/**
 * @file
 * @brief       SHT11 Device Driver Configuration For MSB-430 Platform
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, RIOT
 *
 */
#include <msp430x16x.h>
#include "bitarithm.h"

/* SCK  = P3B5
 * DATA = P3B4
 */

#define SHT11_SCK_LOW   P3OUT &= ~(BIT5);     /**< serial clock line low */
#define SHT11_SCK_HIGH  P3OUT |= BIT5;      /**< serial clock line high */
#define SHT11_DATA      (P3IN & BIT5)      /**< read serial I/O */
#define SHT11_DATA_LOW  P3OUT &= ~(BIT5);   /**< serial I/O line low */
#define SHT11_DATA_HIGH P3OUT |= BIT5;      /**< serial I/O line high */
#define SHT11_DATA_IN   P3DIR &= ~(BIT5);      /**< serial I/O as input */
#define SHT11_DATA_OUT  P3DIR |= BIT5;      /**< serial I/O as output */
#define SHT11_INIT      P3DIR |= BIT5;      /* FIO1DIR |= BIT25; PINSEL3 &= ~(BIT14|BIT15 | BIT16|BIT17); */

/** @} */
#endif /* SHT11BOARD_H_ */
