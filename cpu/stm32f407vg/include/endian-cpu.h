/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

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

#ifndef ENDIANCPU_H_
#define ENDIANCPU_H_

#include <stdint.h>

static __inline uint32_t __byte_reverse_long_asm(uint32_t x)
{
	int tmp;

	__asm __volatile(
		"eor	%1, %2, %2, ROR #16\n"
		"bic	%1, %1, #0x00ff0000\n"
		"mov	%0, %2, ROR #8\n"
		"eor	%0, %0, %1, LSR #8"
	: "=r" (x), "=r" (tmp)
	: "r" (x));

	return (x);
}

static __inline uint16_t __byte_reverse_word_asm(uint16_t x)
{
 	int tmp;
 	uint32_t x32;

 	__asm __volatile(
 		"and	%1, %2, #0xff\n"			// tmp = v & 0xff
 		"mov	%0, %2, lsr #8\n"			// v = v >> 8
 		"orr	%0, %0, %1, lsl #8"			// v |= tmp << 8
 	: "=r" (x32), "=r" (tmp)
 	: "r" (x32));

 	return (uint16_t)x32;
}

#define __byte_reverse_long(x)		__byte_reverse_long_asm(x)
#define __byte_reverse_word(x)		__byte_reverse_word_asm(x)

#endif /* ENDIANCPU_H_ */
