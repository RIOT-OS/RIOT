/**
 *
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @file
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 *
 */

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
