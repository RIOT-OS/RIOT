/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    sys_random Random
 * @ingroup     sys
 * @brief       Pseudo Random Number Generator (PRNG)
 * @{
 *
 * @file
 * @brief       Common interface to the software PRNG
 *
 * Various implementations of a PRNG are available:
 *  - Tiny Mersenne Twister (default)
 *  - Mersenne Twister
 *  - Simple Park-Miller PRNG
 *  - Musl C PRNG
 */

#ifndef RANDOM_H
#define RANDOM_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PRNG_FLOAT
#  define PRNG_FLOAT (0)
#endif

/**
 * @brief initializes PRNG with a seed
 *
 * @warning Currently, the random module uses a global state
 * => multiple calls to @ref random_init will reset the existing
 * state of the PRNG.
 *
 * @param s seed for the PRNG
 */
void random_init(uint32_t s);

/**
 * @brief initialize by an array with array-length
 * init_key is the array for initializing keys
 * key_length is its length
 * slight change for C++, 2004/2/26
 *
 * @param init_key array of keys (seeds) to initialize the PRNG
 * @param key_length number of lements in init_key
 */
void random_init_by_array(uint32_t init_key[], int key_length);

/**
 * @brief generates a random number on [0,0xffffffff]-interval
 * @return a random number on [0,0xffffffff]-interval
 */
uint32_t random_uint32(void);

/**
 * @brief   generates a random number r with a <= r < b.
 *
 * @param[in] a minimum for random number
 * @param[in] b upper bound for random number
 *
 * @pre     a < b
 *
 * @return  a random number on [a,b)-interval
 */
static inline uint32_t random_uint32_range(uint32_t a, uint32_t b)
{
    return (random_uint32() % (b - a)) + a;
}

#if PRNG_FLOAT
/* These real versions are due to Isaku Wada, 2002/01/09 added */

/**
 * @brief generates a random number on [0,1)-real-interval
 * @return a random number on [0,1)-real-interval
 */
double random_real(void);

/**
 * @brief generates a random number on [0,1]-real-interval
 * @return a random number on [0,1]-real-interval
 */
double random_real_inclusive(void);

/**
 * @brief generates a random number on (0,1)-real-interval
 * @return a random number on (0,1)-real-interval
 */
double random_real_exclusive(void);

/**
 * @brief generates a random number on [0,1) with 53-bit resolution
 * @return a random number on [0,1) with 53-bit resolution
 */
double random_res53(void);

#endif /* PRNG_FLOAT */

#ifdef __cplusplus
}
#endif

#endif /* RANDOM_H */
/** @} */
