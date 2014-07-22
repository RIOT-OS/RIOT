/*
 * Copyright (C) 2014 Freie Universität Berlin, Computer Systems & Telematics
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        helper.h
 * @brief       helper functions for sys_crypto_modes
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 */

#ifndef __CRYPTO_MODES_HELPER_H_
#define __CRYPTO_MODES_HELPER_H_

#include "kernel.h"

#define min(a,b) ((a) < (b) ? (a) : (b))

/**
 * @brief Increment a counter encoded in an 16 octet block. The counter is
 *        encoded from the least significant bit in the following form:
 *        block[15-L..15])
 *
 * @param block     encoded block
 * @param L         length of counter
 */
static inline void block_inc_ctr(uint8_t block[16], uint8_t L)
{
	for (uint8_t i = 15; i > 15 - L && !++block[i]; --i)
		;
}


/**
 * @brief   Compares two block of same size
 *
 * @param a     block a
 * @param b     block b
 * @param len   size of both blocks
 */
static inline int equals(uint8_t* a, uint8_t* b, size_t len)
{
	int result = 1;
	while (len-- && (result &= *a++ == *b++))
		;

	return result;
}

#endif /* __CRYPTO_MODES_HELPER_H_ */
