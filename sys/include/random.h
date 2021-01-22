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
 *
 * To use add `USEMODULE += random` to the application Makefile.
 *
 * ## Auto-initialization ##
 *
 * Auto-initialization of the random module is controlled by adding the module
 * `auto_init_random` (as long as `auto_init` is also used). This module is
 * enabled by default when `random` is included, to disable it one must
 * explicitly add it to `DISABLE_MODULE` in the application Makefile. During
 * auto-initialization `auto_init_random` is called, and the PRNG system
 * is seeded.
 *
 * ## PRNG seed sources ##
 *
 * The random module allows different sources to be used for the seed during
 * the auto-initialization (or if `auto_init_random` is used manually). This
 * is controlled by selecting from the following modules:
 *
 * | Module | Description |
 * | ------ | ----------- |
 * | `random_seed_hwrng` | Uses the HWRNG peripheral. |
 * | `random_seed_puf_sram` | Uses the SRAM based physically unclonable function. |
 * | `random_seed_luid` | Uses the Locally Unique ID Generator module, which uses the CPUID peripheral if present. |
 * | `random_seed_fixed` | Uses @ref RANDOM_SEED_DEFAULT as seed. |
 *
 ***WARNING:** `random_seed_luid` and `random_seed_fixed` are not recommended as
 * they provide no real entropy.
 *
 * ### Default behaviour ###
 *
 * The default behaviour, when there is no selected source, is as follows:
 *
 * 1. If the @ref sys_puf_sram module is used `random_seed_puf_sram` is used.
 * 2. If the platform has a HWRNG peripheral `random_seed_hwrng` is used.
 * 3. If the platform has a CPUID peripheral `random_seed_luid` is used.
 * 4. If none of the above applies `random_seed_fixed` is used.
 *
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
 *  - Fortuna (CS)PRNG
 *  - Hardware Random Number Generator (non-seedable)
 *    HWRNG differ in how they generate random numbers and may not use a PRNG internally.
 *    Refer to the manual of your MCU for details.
 */

#ifndef RANDOM_H
#define RANDOM_H

#include <inttypes.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RANDOM_SEED_DEFAULT
/**
 * @brief   Seed selected when all tries to collect seeds from a random source
 *          failed
 */
#define RANDOM_SEED_DEFAULT (1)
#endif

/**
 * @brief Enables support for floating point random number generation
 */
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
 * @brief writes random bytes in the [0,0xff]-interval to memory
 */
void random_bytes(uint8_t *buf, size_t size);

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
uint32_t random_uint32_range(uint32_t a, uint32_t b);

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
